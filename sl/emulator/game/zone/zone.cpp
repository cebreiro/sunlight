#include "zone.h"

#include "sl/emulator/game/entity/game_player.h"
#include "sl/emulator/game/zone/stage.h"
#include "sl/emulator/game/zone/service/game_entity_id_publisher.h"
#include "sl/emulator/game/zone/service/game_item_unique_id_publisher.h"
#include "sl/emulator/server/client/game_client.h"
#include "sl/emulator/server/packet/creator/zone_packet_s2c_creator.h"
#include "sl/emulator/service/gamedata/gamedata_provide_service.h"
#include "sl/emulator/service/snowflake/snowflake_service.h"

namespace sunlight
{
    Zone::Zone(const ServiceLocator& serviceLocator, execution::IExecutor& executor, int32_t id)
        : _serviceLocator(serviceLocator)
        , _strand(std::make_shared<Strand>(executor.SharedFromThis()))
        , _id(id)
    {
        std::optional<int32_t> snowflakeValue = _serviceLocator.Get<SnowflakeService>().Publish(SnowflakeCategory::Item).Get();
        if (!snowflakeValue.has_value())
        {
            throw std::runtime_error(fmt::format("fail to get snowflake value. zone: {}", _id));
        }

        _serviceLocator.Add<GameItemUniqueIdPublisher>(std::make_shared<GameItemUniqueIdPublisher>(_id, *snowflakeValue));
        _serviceLocator.Add<GameEntityIdPublisher>(std::make_shared<GameEntityIdPublisher>(_id));

        _stags.emplace_back(std::make_unique<Stage>(_serviceLocator, id, 10000));
    }

    Zone::~Zone()
    {
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

            stage->SpawnPlayer(player);
            _playerStages[client->GetId()] = stage;

            client->Send(ServerType::Zone, ZonePacketS2CCreator::CreateLoginAccept(*player));
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

    void Zone::HandleNetworkMessage(game_client_id_type id, ZonePacketC2S opcode, UniquePtrNotNull<SlPacketReader> reader)
    {
        Post(*_strand, [self = shared_from_this(), id, opcode, reader = std::move(reader)]() mutable
            {
                self->HandleNetworkMessageImpl(id, opcode, std::move(reader));
            });
    }

    auto Zone::FindStage(int32_t id) -> Stage*
    {
        const auto iter = std::ranges::find_if(_stags, [id](const UniquePtrNotNull<Stage>& stage)
            {
                return stage->GetId() == id;
            });

        return iter != _stags.end() ? iter->get() : nullptr;
    }

    auto Zone::FindStage(int32_t id) const -> const Stage*
    {
        const auto iter = std::ranges::find_if(_stags, [id](const UniquePtrNotNull<Stage>& stage)
            {
                return stage->GetId() == id;
            });

        return iter != _stags.end() ? iter->get() : nullptr;
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

    auto Zone::GetId() const -> int32_t
    {
        return _id;
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
