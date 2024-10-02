#include "player_profile_system.h"

#include "sl/emulator/game/component/player_profile_component.h"
#include "sl/emulator/game/data/sox/zone.h"
#include "sl/emulator/game/entity/game_player.h"
#include "sl/emulator/game/message/character_message.h"
#include "sl/emulator/game/message/zone_message.h"
#include "sl/emulator/game/message/creator/character_message_creator.h"
#include "sl/emulator/game/message/creator/game_player_message_creator.h"
#include "sl/emulator/game/system/game_repository_system.h"
#include "sl/emulator/game/zone/stage.h"
#include "sl/emulator/service/database/dto/profile_introduction.h"
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
        Add(stage.Get<GameRepositorySystem>());
    }

    bool PlayerProfileSystem::Subscribe(Stage& stage)
    {
        if (!stage.AddSubscriber(CharacterMessageType::ShowYou,
            std::bind_front(&PlayerProfileSystem::HandleShowYou, this)))
        {
            return false;
        }

        if (!stage.AddSubscriber(ZoneMessageType::PROFILE_ARCHIVE_MSG,
            std::bind_front(&PlayerProfileSystem::HandleProfileMessage, this)))
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

    void PlayerProfileSystem::OnZoneExit(const GamePlayer& player)
    {
        Get<GameRepositorySystem>().SaveProfile(player);
    }

    void PlayerProfileSystem::OnProfileSettingChange(const ZoneMessage& message)
    {
        GamePlayer& player = message.player;
        SlPacketReader& reader = message.reader;

        const int32_t bitmask = reader.Read<int32_t>();

        PlayerProfileComponent& playerProfileComponent = player.GetProfileComponent();
        playerProfileComponent.SetBitmask(static_cast<int8_t>(bitmask));

        player.Send(GamePlayerMessageCreator::CreateProfileSettingChangeResult(player));
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

    void PlayerProfileSystem::HandleProfileMessage(const ZoneMessage& message)
    {
        GamePlayer& player = message.player;
        SlPacketReader& reader = message.reader;

        const ZoneMessageType subType = reader.Read<ZoneMessageType>();

        switch (subType)
        {
        case ZoneMessageType::PROFILE_ARCHIVE_REQUEST_USER_DATA:
        {
            std::string targetName = reader.ReadString();

            HandleProfileIntroductionRequest(player, targetName);
        }
        break;
        case ZoneMessageType::PROFILE_ARCHIVE_SAVE_USER_DATA:
        {
            HandleProfileIntroductionSave(message);
        }
        break;
        }
    }

    void PlayerProfileSystem::HandleProfileIntroductionRequest(GamePlayer& player, const std::string& targetName)
    {
        const auto iter = _playerNameIndex.find(targetName);
        if (iter == _playerNameIndex.end())
        {
            player.Send(GamePlayerMessageCreator::CreateProfileIntroductionFail(player));

            return;
        }

        GamePlayer& targetPlayer = *iter->second;
        PlayerProfileComponent& targetProfileComponent = targetPlayer.GetProfileComponent();

        if (const std::optional<PlayerProfileIntroduction>& introduction = targetProfileComponent.GetIntroduction();
            introduction.has_value())
        {
            player.Send(GamePlayerMessageCreator::CreateProfileIntroduction(player, *introduction));

            return;
        }

        if (targetProfileComponent.IsLoadPendingIntroduction())
        {
            targetProfileComponent.AddIntroductionLoadingWaiter(player.GetName());

            return;
        }

        targetProfileComponent.SetLoadPendingIntroduction(true);

        Get<GameRepositorySystem>().LoadProfileIntroduction(targetPlayer,
            [this, playerName = player.GetName(), targetName = targetPlayer.GetName()](PlayerProfileIntroduction introduction)
            {
                OnProfileIntroductionLoadComplete(playerName, targetName, introduction);
            });
    }

    void PlayerProfileSystem::OnProfileIntroductionLoadComplete(
        const std::string& playerName, const std::string& targetName, PlayerProfileIntroduction& introduction)
    {
        if (GamePlayer* player = FindPlayer(playerName); player)
        {
            player->Send(GamePlayerMessageCreator::CreateProfileIntroduction(*player, introduction));
        }

        if (GamePlayer* targetPlayer = FindPlayer(targetName); targetPlayer)
        {
            PlayerProfileComponent& targetProfileComponent = targetPlayer->GetProfileComponent();
            assert(targetProfileComponent.IsLoadPendingIntroduction());

            for (const std::string& waiterName : targetProfileComponent.GetIntroductionLoadingWaiters())
            {
                if (GamePlayer* waiter = FindPlayer(waiterName); waiter)
                {
                    if (waiter->GetName() == playerName)
                    {
                        continue;
                    }

                    waiter->Send(GamePlayerMessageCreator::CreateProfileIntroduction(*waiter, introduction));
                }
            }

            targetProfileComponent.ClearIntroductionLoadingWaiters();
            targetProfileComponent.SetLoadPendingIntroduction(false);
            targetProfileComponent.SetIntroduction(std::move(introduction));
        }
    }

    void PlayerProfileSystem::HandleProfileIntroductionSave(const ZoneMessage& message)
    {
        GamePlayer& player = message.player;
        SlPacketReader& reader = message.reader;

        PlayerProfileIntroduction profileIntroduction;

        BufferReader objectReader = reader.ReadObject();

        objectReader.Skip(128);
        profileIntroduction.age = objectReader.ReadString(128);
        profileIntroduction.sex = objectReader.ReadString(128);
        objectReader.Skip(128);
        profileIntroduction.mail = objectReader.ReadString(128);
        profileIntroduction.message = objectReader.ReadString(1024);

        player.GetProfileComponent().SetIntroduction(std::move(profileIntroduction));

        player.Send(GamePlayerMessageCreator::CreateProfileIntroductionSaveResult(player));
    }

    auto PlayerProfileSystem::FindPlayer(const std::string& name) -> GamePlayer*
    {
        const auto iter = _playerNameIndex.find(name);

        return iter != _playerNameIndex.end() ? iter->second : nullptr;
    }

    auto PlayerProfileSystem::FindPlayer(const std::string& name) const -> const GamePlayer*
    {
        const auto iter = _playerNameIndex.find(name);

        return iter != _playerNameIndex.end() ? iter->second : nullptr;
    }
}
