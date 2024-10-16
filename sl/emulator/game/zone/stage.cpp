#include "stage.h"

#include "sl/data/map/map_prop.h"
#include "sl/data/map/map_stage.h"
#include "sl/data/map/map_stage_room.h"
#include "sl/data/map/map_stage_terrain.h"
#include "sl/emulator/game/component/npc_item_shop_component.h"
#include "sl/emulator/game/component/scene_object_component.h"
#include "sl/emulator/game/debug/game_debugger.h"
#include "sl/emulator/game/entity/game_npc.h"
#include "sl/emulator/game/entity/game_player.h"
#include "sl/emulator/game/message/character_message.h"
#include "sl/emulator/game/message/zone_community_message.h"
#include "sl/emulator/game/message/zone_message.h"
#include "sl/emulator/game/message/zone_message_hooker.h"
#include "sl/emulator/game/message/zone_message_type.h"
#include "sl/emulator/game/message/zone_request.h"
#include "sl/emulator/game/system/entity_movement_system.h"
#include "sl/emulator/game/system/entity_status_effect_system.h"
#include "sl/emulator/game/system/entity_view_range_system.h"
#include "sl/emulator/game/system/game_repository_system.h"
#include "sl/emulator/game/system/item_archive_system.h"
#include "sl/emulator/game/system/item_trade_system.h"
#include "sl/emulator/game/system/npc_shop_system.h"
#include "sl/emulator/game/system/player_appearance_system.h"
#include "sl/emulator/game/system/player_channel_system.h"
#include "sl/emulator/game/system/player_group_system.h"
#include "sl/emulator/game/system/player_index_system.h"
#include "sl/emulator/game/system/player_job_system.h"
#include "sl/emulator/game/system/player_profile_system.h"
#include "sl/emulator/game/system/player_quest_system.h"
#include "sl/emulator/game/system/player_skill_effect_system.h"
#include "sl/emulator/game/system/player_state_system.h"
#include "sl/emulator/game/system/player_stat_system.h"
#include "sl/emulator/game/system/scene_object_system.h"
#include "sl/emulator/game/system/server_command_system.h"
#include "sl/emulator/game/time/game_time_service.h"
#include "sl/emulator/game/zone/service/game_entity_id_publisher.h"
#include "sl/emulator/service/gamedata/gamedata_provide_service.h"
#include "sl/emulator/service/gamedata/shop/npc_shop_data_provider.h"

namespace sunlight
{
    Stage::Stage(const ServiceLocator& serviceLocator, int32_t zoneId, const MapStage& stageData)
        : _serviceLocator(serviceLocator)
        , _zoneId(zoneId)
        , _stageData(stageData)
        , _name(fmt::format("stage_{}_{}", _zoneId, _stageData.id))
        , _zoneMessageHooker(std::make_unique<ZoneMessageHooker>(*this))
    {
        InitializeSystem();

        if (stageData.terrain)
        {
            InitializeNPC(stageData.terrain->props);
        }
        else if (stageData.room)
        {
            InitializeNPC(stageData.room->props);
        }
        else
        {
            assert(false);
        }
    }

    Stage::~Stage()
    {
    }

    void Stage::Update()
    {
        if (GameDebugger* debugger = _serviceLocator.Find<GameDebugger>(); debugger && debugger->HasDebugTarget())
        {
            GameDebugger::SetInstance(debugger);
        }

        GameTimeService::SetNow(game_clock_type::now());

        SUNLIGHT_GAME_DEBUG_REPORT_INTERVAL(
            GameDebugType::SceneStatus,
            std::chrono::seconds(30),
            Get<SceneObjectSystem>().GetDebugStatus());

        for (GameSystem& system : _updateSystems | notnull::reference)
        {
            system.Update();
        }

        GameDebugger::SetInstance(nullptr);
    }

    void Stage::HandleNetworkMessage(game_client_id_type id, ZonePacketC2S opcode, UniquePtrNotNull<SlPacketReader> reader)
    {
        const auto iter = _players.find(id);
        if (iter == _players.end())
        {
            return;
        }

        SharedPtrNotNull<GamePlayer> player = iter->second;

        if (GameDebugger* debugger = _serviceLocator.Find<GameDebugger>(); debugger && debugger->HasDebugTarget())
        {
            GameDebugger::SetInstance(debugger);
        }

        GameTimeService::SetNow(game_clock_type::now());

        switch (opcode)
        {
        case ZonePacketC2S::NMC_REQ_MOVE:
        case ZonePacketC2S::NMC_REQ_STOP:
        case ZonePacketC2S::NMC_REQ_SETDIRECTION:
        case ZonePacketC2S::NMC_REQ_CHANGE_ROOM:
        case ZonePacketC2S::NMC_REQ_USERINFO:
        case ZonePacketC2S::NMC_REQ_UNLOCK:
        case ZonePacketC2S::NMC_SEND_MESSAGE_TO:
        case ZonePacketC2S::NMC_SEND_LOCAL_MESSAGE:
        case ZonePacketC2S::NMC_SEND_GLOBAL_MESSAGE:
        case ZonePacketC2S::NMC_LOGIN:
        case ZonePacketC2S::NMC_LOGOUT:
        case ZonePacketC2S::UNKNOWN_0x86:
        {
            ZoneRequest request{
                .player = *player,
                .type = opcode,
                .reader = *reader,
            };

            if (!Publish(request))
            {
                SUNLIGHT_LOG_WARN(_serviceLocator,
                    fmt::format("[{}] unhandled zone request. player: {}, type: {}, buffer: {}",
                        GetName(), player->GetCId(), ToString(opcode), reader->GetBuffer().ToString()));
            }
        }
        break;
        case ZonePacketC2S::NMC_TRIGGER_EVENT:
        {
            const uint8_t eventType = reader->Read<uint8_t>();
            switch (eventType)
            {
            case 0:
            {
                const int32_t msgType = reader->Read<int32_t>();

                ZoneCommunityMessage message{
                    .player = *player,
                    .type = static_cast<ZoneMessageType>(msgType),
                    .reader = *reader,
                };

                if (!Publish(message))
                {
                    SUNLIGHT_LOG_WARN(_serviceLocator,
                        fmt::format("[{}] unhandled zone community message. player: {}, type: {}, buffer: {}",
                            GetName(), player->GetCId(), ToString(message.type), message.reader.GetBuffer().ToString()));
                }
            }
            break;
            case 1:
            {
                // stage message
                [[maybe_unused]]
                const int32_t unk = reader->Read<int32_t>();
                const auto [low, high] = reader->ReadInt64();
                const int32_t msgType = reader->Read<int32_t>();

                ZoneMessage message{
                    .player = *player,
                    .type = static_cast<ZoneMessageType>(msgType),
                    .targetId = game_entity_id_type(low),
                    .targetType = static_cast<GameEntityType>(high),
                    .reader = *reader
                };

                if (!Publish(message))
                {
                    SUNLIGHT_LOG_WARN(_serviceLocator,
                        fmt::format("[{}] unhandled zone message. player: {}, target: [{}, {}], type: {}, buffer: {}",
                            GetName(), player->GetCId(), message.targetId, ToString(message.targetType), ToString(message.type),
                            message.reader.GetBuffer().ToString()));
                }
            }
            break;
            case 3:
            {
                CharacterMessage message{
                    .player = *player,
                    .reader = *reader
                };
                message.targetName = reader->ReadString();
                message.type = static_cast<CharacterMessageType>(reader->Read<int32_t>());

                if (!Publish(message))
                {
                    SUNLIGHT_LOG_WARN(_serviceLocator,
                        fmt::format("[{}] unhandled character message. player: {}, target: {}, type: {}, buffer: {}",
                            GetName(), player->GetCId(), message.targetName, ToString(message.type),
                            message.reader.GetBuffer().ToString()));
                }
            }
            break;
            default:
                SUNLIGHT_LOG_WARN(_serviceLocator,
                    fmt::format("[{}] unhandled zone message. player: {}, event_type: {}, buffer: {}",
                        GetName(), player->GetCId(), eventType, reader->GetBuffer().ToString()));
            }
        }
        break;
        default:
            assert(false);
        }

        GameDebugger::SetInstance(nullptr);
    }

    void Stage::SpawnPlayer(SharedPtrNotNull<GamePlayer> player, StageEnterType enterType)
    {
        assert(!_players.contains(player->GetClientId()));

        _players[player->GetClientId()] = player;

        if (GameDebugger* debugger = _serviceLocator.Find<GameDebugger>(); debugger && debugger->HasDebugTarget())
        {
            GameDebugger::SetInstance(debugger);
        }

        GameTimeService::SetNow(game_clock_type::now());

        Get<PlayerIndexSystem>().OnStageEnter(*player);
        Get<SceneObjectSystem>().SpawnPlayer(player, enterType);
        Get<PlayerChannelSystem>().OnStageEnter(*player, enterType);
        Get<EntityStatusEffectSystem>().OnStageEnter(*player, enterType);

        GameDebugger::SetInstance(nullptr);
    }

    auto Stage::DespawnPlayer(game_client_id_type clientId, StageExitType exitType) -> Future<std::shared_ptr<GamePlayer>>
    {
        std::shared_ptr<GamePlayer> player;

        const auto iter = _players.find(clientId);
        if (iter == _players.end())
        {
            co_return player;
        }

        player = std::move(iter->second);
        _players.erase(iter);

        if (GameDebugger* debugger = _serviceLocator.Find<GameDebugger>(); debugger && debugger->HasDebugTarget())
        {
            GameDebugger::SetInstance(debugger);
        }

        GameTimeService::SetNow(game_clock_type::now());

        Get<SceneObjectSystem>().DespawnPlayer(player->GetId(), exitType);
        Get<NPCShopSystem>().OnStageExit(*player);
        Get<PlayerGroupSystem>().OnStageExit(*player);
        Get<PlayerChannelSystem>().OnStageExit(*player, exitType);
        Get<EntityStatusEffectSystem>().OnStageExit(*player, exitType);

        switch (exitType)
        {
        case StageExitType::Logout:
        case StageExitType::ZoneChange:
        {
            Get<PlayerProfileSystem>().OnZoneExit(*player);
        }
        break;
        case StageExitType::StageChange:
            break;
        }

        Get<PlayerIndexSystem>().OnStageExit(*player);

        GameDebugger::SetInstance(nullptr);

        // to ignore client network message during exit
        co_await Delay(std::chrono::milliseconds(1000));

        co_return player;
    }

    bool Stage::AddSubscriber(ZonePacketC2S type, const std::function<void(const ZoneRequest&)>& subscriber)
    {
        return _zoneRequestSubscribers.try_emplace(type, subscriber).second;
    }

    bool Stage::AddSubscriber(ZoneMessageType type, const std::function<void(const ZoneMessage&)>& subscriber)
    {
        return _zoneMessageSubscribers.try_emplace(type, subscriber).second;
    }

    bool Stage::AddSubscriber(ZoneMessageType type, const std::function<void(const ZoneCommunityMessage&)>& subscriber)
    {
        return _zoneCommunityMessageSubscribers.try_emplace(type, subscriber).second;
    }

    bool Stage::AddSubscriber(CharacterMessageType type, const std::function<void(const CharacterMessage&)>& subscriber)
    {
        return _characterMessageSubscriber.try_emplace(type, subscriber).second;
    }

    auto Stage::GetId() const -> int32_t
    {
        return _stageData.id;
    }

    auto Stage::GetName() const -> const std::string&
    {
        return _name;
    }

    auto Stage::GetServiceLocator() const -> const ServiceLocator&
    {
        return _serviceLocator;
    }

    template <typename T> requires std::derived_from<T, GameSystem>
    bool Stage::Add(SharedPtrNotNull<T> system)
    {
        assert(system);

        const auto& id = GameSystem::GetClassId<T>();

        const auto& [iter, inserted] = _systems.try_emplace(id, nullptr);
        if (inserted)
        {
            iter->second = std::move(system);

            if (iter->second->ShouldUpdate())
            {
                _updateSystems.emplace_back(iter->second.get());
            }
        }

        return inserted;
    }

    void Stage::InitializeSystem()
    {
        Add(std::make_shared<SceneObjectSystem>(_serviceLocator, _stageData.id));
        Add(std::make_shared<EntityViewRangeSystem>(_serviceLocator, _stageData));
        Add(std::make_shared<EntityMovementSystem>());
        Add(std::make_shared<ServerCommandSystem>(_serviceLocator, _stageData.id));
        Add(std::make_shared<ItemArchiveSystem>(_serviceLocator));
        Add(std::make_shared<PlayerStatSystem>(_serviceLocator));
        Add(std::make_shared<PlayerJobSystem>(_serviceLocator, _stageData.id));
        Add(std::make_shared<PlayerStateSystem>(_serviceLocator, _stageData));
        Add(std::make_shared<PlayerAppearanceSystem>(_serviceLocator));
        Add(std::make_shared<GameRepositorySystem>(_serviceLocator));
        Add(std::make_shared<PlayerQuestSystem>());
        Add(std::make_shared<NPCShopSystem>(_serviceLocator));
        Add(std::make_shared<PlayerProfileSystem>(_serviceLocator, _zoneId));
        Add(std::make_shared<PlayerGroupSystem>(_serviceLocator));
        Add(std::make_shared<ItemTradeSystem>(_serviceLocator));
        Add(std::make_shared<PlayerChannelSystem>(_serviceLocator, _zoneId));
        Add(std::make_shared<PlayerIndexSystem>());
        Add(std::make_shared<PlayerSkillEffectSystem>(_serviceLocator));
        Add(std::make_shared<EntityStatusEffectSystem>(_serviceLocator, _stageData.id));

        const auto range = _systems | std::views::values;

        for (const SharedPtrNotNull<GameSystem>& system : range)
        {
            system->InitializeSubSystem(*this);
        }

        for (const SharedPtrNotNull<GameSystem>& system : range)
        {
            system->Subscribe(*this);
        }

        for (const SharedPtrNotNull<GameSystem>& system : range)
        {
            if (system->HasCyclicSystemDependency())
            {
                assert(false);

                throw std::runtime_error("system cyclic dependency detected!!");
            }
        }
    }

    void Stage::InitializeNPC(const std::vector<MapProp>& props)
    {
        const NPCShopDataProvider& npcShopDataProvider = _serviceLocator.Get<GameDataProvideService>().GetNPCShopDataProvider();
        GameEntityIdPublisher& entityIdPublisher = _serviceLocator.Get<GameEntityIdPublisher>();

        for (const MapProp& prop : props)
        {
            if (prop.type == static_cast<int32_t>(GameEntityType::NPC))
            {
                const auto [pos, yaw] = ExtractPositionAndYaw(prop.position);

                auto sceneObjectComponent = std::make_unique<SceneObjectComponent>();
                sceneObjectComponent->SetId(entityIdPublisher.PublishSceneObjectId(GameEntityType::NPC));
                sceneObjectComponent->SetPosition(Eigen::Vector2f(pos.x(), pos.y()));
                sceneObjectComponent->SetDestPosition(sceneObjectComponent->GetPosition());
                sceneObjectComponent->SetYaw(yaw);

                auto npc = std::make_shared<GameNPC>(game_entity_id_type(prop.id), prop.pnx);
                npc->AddComponent(std::move(sceneObjectComponent));

                if (prop.unk2)
                {
                    if (const ItemShopData* itemShopData = npcShopDataProvider.FindItemShopData(prop.unk2); itemShopData)
                    {
                        npc->AddComponent(std::make_unique<NPCItemShopComponent>(*itemShopData));

                        Get<NPCShopSystem>().InitializeItemShop(*npc);
                    }

                    if (const HairShopData* hairShopData = npcShopDataProvider.FindHairShopData(prop.unk2); hairShopData)
                    {
                        
                    }
                }

                Get<SceneObjectSystem>().SpawnNPC(std::move(npc));
            }
        }
    }

    void Stage::InitializeNPC(const std::vector<MapTerrainProp>& props)
    {
        const NPCShopDataProvider& npcShopDataProvider = _serviceLocator.Get<GameDataProvideService>().GetNPCShopDataProvider();
        GameEntityIdPublisher& entityIdPublisher = _serviceLocator.Get<GameEntityIdPublisher>();

        for (const MapTerrainProp& prop : props)
        {
            if (prop.type == static_cast<int32_t>(GameEntityType::NPC))
            {
                const auto [pos, yaw] = ExtractPositionAndYaw(prop.position);

                auto sceneObjectComponent = std::make_unique<SceneObjectComponent>();
                sceneObjectComponent->SetId(entityIdPublisher.PublishSceneObjectId(GameEntityType::NPC));
                sceneObjectComponent->SetPosition(Eigen::Vector2f(pos.x(), pos.y()));
                sceneObjectComponent->SetDestPosition(sceneObjectComponent->GetPosition());
                sceneObjectComponent->SetYaw(yaw);

                auto npc = std::make_shared<GameNPC>(game_entity_id_type(prop.id), prop.pnx);
                npc->AddComponent(std::move(sceneObjectComponent));
                npc->SetUnk1(prop.unk1);
                npc->SetUnk2(prop.unk2);

                if (prop.unk6)
                {
                    if (const ItemShopData* itemShopData = npcShopDataProvider.FindItemShopData(prop.unk6); itemShopData)
                    {
                        npc->AddComponent(std::make_unique<NPCItemShopComponent>(*itemShopData));

                        Get<NPCShopSystem>().InitializeItemShop(*npc);
                    }

                    if (const HairShopData* hairShopData = npcShopDataProvider.FindHairShopData(prop.unk6); hairShopData)
                    {

                    }
                }

                Get<SceneObjectSystem>().SpawnNPC(std::move(npc));
            }
        }
    }

    bool Stage::Publish(const ZoneRequest& request)
    {
        const auto iter = _zoneRequestSubscribers.find(request.type);
        if (iter == _zoneRequestSubscribers.end())
        {
            return false;
        }

        iter->second(request);

        return true;
    }

    bool Stage::Publish(const ZoneMessage& message)
    {
        if (_zoneMessageHooker->ProcessEvent(message))
        {
            return true;
        }

        const auto iter = _zoneMessageSubscribers.find(message.type);
        if (iter == _zoneMessageSubscribers.end())
        {
            return false;
        }

        iter->second(message);

        return true;
    }

    bool Stage::Publish(const ZoneCommunityMessage& message)
    {
        const auto iter = _zoneCommunityMessageSubscribers.find(message.type);
        if (iter == _zoneCommunityMessageSubscribers.end())
        {
            return false;
        }

        iter->second(message);

        return true;
    }

    bool Stage::Publish(const CharacterMessage& message)
    {
        const auto iter = _characterMessageSubscriber.find(message.type);
        if (iter == _characterMessageSubscriber.end())
        {
            return false;
        }

        iter->second(message);

        return true;
    }

    auto Stage::ExtractPositionAndYaw(const Eigen::Matrix4f& matrix) -> std::pair<Eigen::Vector3f, float>
    {
        std::pair<Eigen::Vector3f, float> result;
        result.first = matrix.block<1, 3>(3, 0).transpose();
        result.second = std::atan2f(matrix(1, 0), matrix(0, 0)) * 180.0f / static_cast<float>(std::numbers::pi);

        return result;
    }
}
