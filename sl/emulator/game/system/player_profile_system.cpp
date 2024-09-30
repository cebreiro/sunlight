#include "player_profile_system.h"

#include "sl/emulator/game/data/sox/zone.h"
#include "sl/emulator/game/entity/game_player.h"
#include "sl/emulator/game/message/character_message.h"
#include "sl/emulator/game/message/creator/character_message_creator.h"
#include "sl/emulator/game/zone/stage.h"
#include "sl/emulator/service/gamedata/gamedata_provide_service.h"

namespace sunlight
{
    PlayerProfileSystem::PlayerProfileSystem(const ServiceLocator& serviceLocator, int32_t zoneId)
        : _serviceLocator(serviceLocator)
        , _zoneData([&]() -> const sox::Zone&
            {
                std::string str = std::to_string(zoneId);

                for (const sox::Zone& data : serviceLocator.Get<GameDataProvideService>().Get<sox::ZoneTable>().Get())
                {
                    if (data.zoneFilename == str)
                    {
                        return data;
                    }
                }

                throw std::runtime_error(fmt::format("fail to find sox zone data. zone: {}", zoneId));
            }())
    {
    }

    void PlayerProfileSystem::InitializeSubSystem(Stage& stage)
    {
        (void)stage;
    }

    bool PlayerProfileSystem::Subscribe(Stage& stage)
    {
        if (!stage.AddSubscriber(CharacterMessageType::ShowYou,
            std::bind_front(&PlayerProfileSystem::HandleShowYou, this)))
        {
            return false;
        }

        return true;
    }

    auto PlayerProfileSystem::GetName() const -> std::string_view
    {
        return "player_profile_system";
    }

    auto PlayerProfileSystem::GetClassId() const -> game_system_id_type
    {
        return GameSystem::GetClassId<PlayerProfileSystem>();
    }

    void PlayerProfileSystem::OnStageEnter(GamePlayer& player)
    {
        if (!_playerNameIndex.try_emplace(player.GetName(), &player).second)
        {
            SUNLIGHT_LOG_CRITICAL(_serviceLocator,
                fmt::format("[{}] fail to insert player name. player: {}, name: {}",
                    GetName(), player.GetCId(), player.GetName()));
        }
    }

    void PlayerProfileSystem::OnStageExit(const GamePlayer& player)
    {
        [[maybe_unused]]
        const size_t erased = _playerNameIndex.erase(player.GetName());
        assert(erased > 0);
    }

    void PlayerProfileSystem::HandleShowYou(const CharacterMessage& message)
    {
        GamePlayer& player = message.player;

        const auto iter = _playerNameIndex.find(message.targetName);
        if (iter == _playerNameIndex.end())
        {
            player.Send(CharacterMessageCreator::CreateProfileFail(message.targetName));
        }
        else
        {
            player.Send(CharacterMessageCreator::CreateProfile(*iter->second, _zoneData));
        }
    }
}
