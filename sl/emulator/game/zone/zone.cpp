#include "zone.h"

#include "sl/emulator/game/game_constant.h"
#include "sl/emulator/game/component/scene_object_component.h"
#include "sl/emulator/game/debug/game_debugger.h"
#include "sl/emulator/game/entity/game_player.h"
#include "sl/emulator/game/system/game_repository_system.h"
#include "sl/emulator/game/message/creator/normal_message_creator.h"
#include "sl/emulator/game/script/lua_script_engine.h"
#include "sl/emulator/game/zone/stage.h"
#include "sl/emulator/game/zone/service/game_community_service.h"
#include "sl/emulator/game/zone/service/game_entity_id_publisher.h"
#include "sl/emulator/game/zone/service/game_item_unique_id_publisher.h"
#include "sl/emulator/game/zone/service/zone_change_service.h"
#include "sl/emulator/server/client/game_client.h"
#include "sl/emulator/server/packet/creator/zone_packet_s2c_creator.h"
#include "sl/emulator/service/community/community_service.h"
#include "sl/emulator/service/community/command/community_command.h"
#include "sl/emulator/service/gamedata/gamedata_provide_service.h"
#include "sl/emulator/service/gamedata/map/map_data_provider.h"
#include "sl/emulator/service/snowflake/snowflake_service.h"

namespace sunlight
{
    Zone::Zone(const ServiceLocator& serviceLocator, execution::IExecutor& executor, int8_t worldId, int32_t id)
        : _serviceLocator(serviceLocator)
        , _strand(std::make_shared<Strand>(executor.SharedFromThis()))
        , _worldId(worldId)
        , _id(id)
    {
        std::optional<int32_t> snowflakeValue = _serviceLocator.Get<SnowflakeService>().Publish(SnowflakeCategory::Item).Get();
        if (!snowflakeValue.has_value())
        {
            throw std::runtime_error(fmt::format("fail to get snowflake value. zone: {}", _id));
        }

        _serviceLocator.Add<LuaScriptEngine>(std::make_shared<LuaScriptEngine>(_serviceLocator,
            _serviceLocator.Get<GameDataProvideService>().GetScriptPath()));
        _serviceLocator.Add<GameDebugger>(std::make_shared<GameDebugger>());
        _serviceLocator.Add<GameItemUniqueIdPublisher>(std::make_shared<GameItemUniqueIdPublisher>(_id, *snowflakeValue));
        _serviceLocator.Add<GameEntityIdPublisher>(std::make_shared<GameEntityIdPublisher>(_id));
        _serviceLocator.Add<ZoneChangeService>(std::make_shared<ZoneChangeService>(*this));

        auto communityCommandChannel = std::make_shared<Channel<SharedPtrNotNull<ICommunityCommand>>>();
        auto communityNotificationChannel = _serviceLocator.Get<CommunityService>()
            .StartStreaming(_id, AsyncEnumerable<SharedPtrNotNull<ICommunityCommand>>(communityCommandChannel));

        _gameCommunityService = std::make_shared<GameCommunityService>(*this, std::move(communityCommandChannel), communityNotificationChannel.GetChannel());
        _gameCommunityService->Start();

        _serviceLocator.Add<GameCommunityService>(_gameCommunityService);

        const MapDataProvider& mapDataProvider = _serviceLocator.Get<GameDataProvideService>().GetMapDataProvider();
        if (_mapData = mapDataProvider.FindMap(id); !_mapData)
        {
            throw std::runtime_error(fmt::format("fail to find map data. zone: {}", _id));
        }

        for (const MapStage& stageData : _mapData->stages | notnull::reference)
        {
            _stages.emplace_back(std::make_unique<Stage>(_serviceLocator, id, stageData));
        }
    }

    Zone::~Zone()
    {
    }

    void Zone::Start()
    {
        Post(*_strand, [self = shared_from_this()]()
            {
                self->_updateFuture = self->Update();
            });
    }

    void Zone::Shutdown()
    {
        _shutdown.store(true);

        _gameCommunityService->Shutdown();
    }

    auto Zone::Join() -> Future<void>
    {
        if (_updateFuture.IsValid())
        {
            co_await _updateFuture;

            _updateFuture = Future<void>();
        }

        co_await _gameCommunityService->Join();

        co_return;
    }

    auto Zone::SpawnPlayer(SharedPtrNotNull<GameClient> client, db::dto::Character dto) -> Future<bool>
    {
        [[maybe_unused]]
        const auto self = shared_from_this();

        co_await *_strand;
        assert(_strand);

        try
        {
            auto player = std::make_shared<GamePlayer>(client, dto,
                _serviceLocator.Get<GameDataProvideService>(),
                _serviceLocator.Get<GameEntityIdPublisher>());

            Stage* stage = FindStage(dto.stage);
            if (!stage)
            {
                throw std::runtime_error(fmt::format("fail to find stage. stage: {}", dto.stage));
            }

            stage->SpawnPlayer(player, StageEnterType::Login);

            assert(!_playerStages.contains(client->GetId()));
            assert(!_clients.contains(client->GetId()));

            _playerStages[client->GetId()] = stage;
            _playerCIdStageIndex[dto.id] = stage;
            _clients[client->GetId()] = client;
        }
        catch (const std::exception& e)
        {
            SUNLIGHT_LOG_ERROR(_serviceLocator,
                fmt::format("[zone_{}] fail to spawn player. cid: {}, exception: {}",
                    GetId(), dto.id, e.what()));

            client->Send(ServerType::Zone, ZonePacketS2CCreator::CreateLoginReject("internal error"));

            co_return false;
        }

        co_return true;
    }

    auto Zone::LogoutPlayer(game_client_id_type id) -> Future<bool>
    {
        [[maybe_unused]]
        const auto self = shared_from_this();

        co_await *_strand;
        assert(_strand);

        const auto iter = _playerStages.find(id);
        if (iter == _playerStages.end())
        {
            co_return false;
        }

        Stage& stage = *iter->second;

        const std::shared_ptr<GamePlayer> player = co_await stage.DespawnPlayer(id, StageExitType::Logout);
        if (!player)
        {
            co_return false;
        }

        const SceneObjectComponent& sceneObjectComponent = player->GetSceneObjectComponent();
        const Eigen::Vector2f& position = sceneObjectComponent.GetPosition();

        GameRepositorySystem& repositorySystem = stage.Get<GameRepositorySystem>();
        repositorySystem.SaveState(*player, _id, stage.GetId(), position.x(), position.y(), sceneObjectComponent.GetYaw());

        co_await repositorySystem.WaitForSaveCompletion(*player);

        _playerCIdStageIndex.erase(player->GetCId());

        [[maybe_unused]]
        const size_t erased = _clients.erase(id);
        assert(erased > 0);

        _playerStages.erase(iter);

        co_return true;
    }

    auto Zone::RemovePlayerByZoneChange(game_client_id_type id, int32_t destZoneId, float x, float y, float yaw) -> Future<bool>
    {
        assert(ExecutionContext::IsEqualTo(*_strand));

        [[maybe_unused]]
        const auto self = shared_from_this();

        const auto iter = _playerStages.find(id);
        if (iter == _playerStages.end())
        {
            assert(false);

            co_return false;
        }

        Stage& stage = *iter->second;

        const std::shared_ptr<GamePlayer> player = co_await stage.DespawnPlayer(id, StageExitType::ZoneChange);
        if (!player)
        {
            assert(false);

            co_return false;
        }

        _playerCIdStageIndex.erase(player->GetCId());
        _playerStages.erase(iter);

        GameRepositorySystem& repositorySystem = stage.Get<GameRepositorySystem>();
        repositorySystem.SaveState(*player, destZoneId, GameConstant::STAGE_MAIN, x, y, yaw);

        co_await repositorySystem.WaitForSaveCompletion(*player);

        [[maybe_unused]]
        const size_t erased =_clients.erase(id);
        assert(erased > 0);

        co_return true;
    }

    auto Zone::ChangePlayerStage(game_client_id_type id, int32_t destStageId, int32_t destX, int32_t destY) -> Future<bool>
    {
        assert(ExecutionContext::IsEqualTo(*_strand));

        [[maybe_unused]]
        const auto self = shared_from_this();

        Stage* destStage = FindStage(destStageId);
        if (!destStage)
        {
            co_return false;
        }

        const auto iter = _playerStages.find(id);
        if (iter == _playerStages.end())
        {
            assert(false);

            co_return false;
        }

        Stage& srcStage = *iter->second;

        std::shared_ptr<GamePlayer> player = co_await srcStage.DespawnPlayer(id, StageExitType::StageChange);
        if (!player)
        {
            assert(false);

            co_return false;
        }

        const int64_t cid = player->GetCId();

        player->Send(NormalMessageCreator::CreateChangeRoom(destStageId, destX, destY));

        SceneObjectComponent& sceneObjectComponent = player->GetSceneObjectComponent();
        sceneObjectComponent.SetPosition(Eigen::Vector2f(static_cast<float>(destX), static_cast<float>(destY)));
        sceneObjectComponent.SetDestPosition(sceneObjectComponent.GetPosition());

        destStage->SpawnPlayer(std::move(player), StageEnterType::StageChange);

        iter->second = destStage;
        _playerCIdStageIndex[cid] = destStage;

        co_return true;
    }

    void Zone::HandleClientDisconnect(game_client_id_type id)
    {
        LogoutPlayer(id);
    }

    void Zone::HandleNetworkMessage(game_client_id_type id, ZonePacketC2S opcode, UniquePtrNotNull<SlPacketReader> reader)
    {
        Post(*_strand, [self = shared_from_this(), id, opcode, reader = std::move(reader)]() mutable
            {
                self->HandleNetworkMessageImpl(id, opcode, std::move(reader));
            });
    }

    auto Zone::Update() -> Future<void>
    {
        [[maybe_unused]]
        const auto self = shared_from_this();

        while (true)
        {
            // TODO: await(or slow down tick interval) player entrance if zone has no player

            for (Stage& stage : _stages | notnull::reference)
            {
                stage.Update();
            }

            co_await Delay(std::chrono::milliseconds(200));
            assert(ExecutionContext::IsEqualTo(*_strand));

            if (_shutdown)
            {
                break;
            }
        }

        co_return;
    }

    auto Zone::FindStage(int32_t id) -> Stage*
    {
        const auto iter = std::ranges::find_if(_stages, [id](const UniquePtrNotNull<Stage>& stage)
            {
                return stage->GetId() == id;
            });

        return iter != _stages.end() ? iter->get() : nullptr;
    }

    auto Zone::FindStage(int32_t id) const -> const Stage*
    {
        const auto iter = std::ranges::find_if(_stages, [id](const UniquePtrNotNull<Stage>& stage)
            {
                return stage->GetId() == id;
            });

        return iter != _stages.end() ? iter->get() : nullptr;
    }

    auto Zone::FindPlayerStage(int64_t cid) -> Stage*
    {
        const auto iter = _playerCIdStageIndex.find(cid);

        return iter != _playerCIdStageIndex.end() ? iter->second : nullptr;
    }

    auto Zone::FindPlayerStage(int64_t cid) const -> const Stage*
    {
        const auto iter = _playerCIdStageIndex.find(cid);

        return iter != _playerCIdStageIndex.end() ? iter->second : nullptr;
    }

    auto Zone::FindClient(game_client_id_type id) -> GameClient*
    {
        const auto iter = _clients.find(id);

        return iter != _clients.end() ? iter->second.get() : nullptr;
    }

    auto Zone::FindClient(game_client_id_type id) const -> const GameClient*
    {
        const auto iter = _clients.find(id);

        return iter != _clients.end() ? iter->second.get() : nullptr;
    }

    auto Zone::GetServiceLocator() const -> const ServiceLocator&
    {
        return _serviceLocator;
    }

    auto Zone::GetStrand() -> Strand&
    {
        return *_strand;
    }

    auto Zone::GetStrand() const -> const Strand&
    {
        return *_strand;
    }

    auto Zone::GetWorldId() const -> int8_t
    {
        return _worldId;
    }

    auto Zone::GetId() const -> int32_t
    {
        return _id;
    }

    auto Zone::GetStages() const -> std::span<const UniquePtrNotNull<Stage>>
    {
        return _stages;
    }

    void Zone::HandleNetworkMessageImpl(game_client_id_type id, ZonePacketC2S opcode, UniquePtrNotNull<SlPacketReader> reader)
    {
        const auto iter = _playerStages.find(id);
        if (iter == _playerStages.end())
        {
            assert(false);

            return;
        }

        iter->second->HandleNetworkMessage(id, opcode, std::move(reader));
    }
}
