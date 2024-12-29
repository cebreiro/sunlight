#include "player_appearance_system.h"

#include "sl/generator/game/component/player_appearance_component.h"
#include "sl/generator/game/component/player_item_component.h"
#include "sl/generator/game/component/player_job_component.h"
#include "sl/generator/game/data/sox/item_weapon.h"
#include "sl/generator/game/entity/game_item.h"
#include "sl/generator/game/entity/game_player.h"
#include "sl/generator/game/message/zone_message.h"
#include "sl/generator/game/message/creator/game_player_message_creator.h"
#include "sl/generator/game/system/entity_view_range_system.h"
#include "sl/generator/game/zone/stage.h"
#include "sl/generator/game/zone/service/game_repository_service.h"
#include "sl/generator/service/gamedata/item/item_data.h"

namespace sunlight
{
    PlayerAppearanceSystem::PlayerAppearanceSystem(const ServiceLocator& serviceLocator)
        : _serviceLocator(serviceLocator)
    {
    }

    void PlayerAppearanceSystem::InitializeSubSystem(Stage& stage)
    {
        Add(stage.Get<EntityViewRangeSystem>());
    }

    bool PlayerAppearanceSystem::Subscribe(Stage& stage)
    {
        if (!stage.AddSubscriber(ZoneMessageType::MULTIPLAYER_SYNC_MSG,
            std::bind_front(&PlayerAppearanceSystem::HandleMultiPlayerSyncMessage, this)))
        {
            return false;
        }

        return true;
    }

    auto PlayerAppearanceSystem::GetName() const -> std::string_view
    {
        return "player_appearance_system";
    }

    auto PlayerAppearanceSystem::GetClassId() const -> game_system_id_type
    {
        return GameSystem::GetClassId<PlayerAppearanceSystem>();
    }

    void PlayerAppearanceSystem::UpdateEquipmentAppearance(GamePlayer& player)
    {
        constexpr std::array appearanceEquipPositions = {
            EquipmentPosition::Hat,
            EquipmentPosition::Jacket,
            EquipmentPosition::Gloves,
            EquipmentPosition::Pants,
            EquipmentPosition::Shoes,
            EquipmentPosition::Weapon1,
        };

        const PlayerItemComponent& itemComponent = player.GetItemComponent();
        PlayerAppearanceComponent& appearanceComponent = player.GetAppearanceComponent();

        for (const EquipmentPosition position : appearanceEquipPositions)
        {
            const GameItem* item = itemComponent.FindEquipmentItem(position);

            const int32_t modelId = item ? item->GetData().GetModelId() : 0;
            const int32_t modelColor = item ? item->GetData().GetModelColor() : 0;

            switch (position)
            {
            case EquipmentPosition::Hat:
            {
                appearanceComponent.SetHatModelId(modelId);
                appearanceComponent.SetHatModelColor(modelColor);
            }
            break;
            case EquipmentPosition::Jacket:
            {
                appearanceComponent.SetJacketModelId(modelId);
                appearanceComponent.SetJacketModelColor(modelColor);
            }
            break;
            case EquipmentPosition::Gloves:
            {
                appearanceComponent.SetGlovesModelId(modelId);
                appearanceComponent.SetGlovesModelColor(modelColor);
            }
            break;
            case EquipmentPosition::Pants:
            {
                appearanceComponent.SetPantsModelId(modelId);
                appearanceComponent.SetPantsModelColor(modelColor);
            }
            break;
            case EquipmentPosition::Shoes:
            {
                appearanceComponent.SetShoesModelId(modelId);
                appearanceComponent.SetShoesModelColor(modelColor);
            }
            break;
            case EquipmentPosition::Weapon1:
            {
                appearanceComponent.SetWeaponModelId(modelId);
                appearanceComponent.SetWeaponModelColor(modelColor);
            }
            break;
            default:
                assert(false);
            }
        }

        appearanceComponent.SetWeaponMotionCategory(GetWeaponMotionCategory(player));
    }

    void PlayerAppearanceSystem::HandleMultiPlayerSyncMessage(const ZoneMessage& message)
    {
        GamePlayer& player = message.player;
        SlPacketReader& reader = message.reader;
        game_entity_id_type targetId = message.targetId;
        GameEntityType targetType = message.targetType;

        const auto subType = reader.Read<ZoneMessageType>();

        switch (subType)
        {
        case ZoneMessageType::MULTIPLAYER_SYNC_CHANGE_HAIR_COLOR:
        {
            const int32_t newColor = reader.Read<int32_t>();

            HandleHairColorChange(player, newColor);
        }
        break;
        case ZoneMessageType::MULTIPLAYER_SYNC_CHANGE_HAIR:
        {
            const int32_t newHair = reader.Read<int32_t>();

            HandleHairChange(player, newHair);
        }
        break;
        default:
            SUNLIGHT_LOG_WARN(_serviceLocator,
                fmt::format("[{}] multiplayer message. player: {}, type: {}, target: [{}, {}], buffer: {}",
                    GetName(), player.GetCId(), ToString(subType), targetId, ToString(targetType), reader.GetBuffer().ToString()));
        }

    }

    void PlayerAppearanceSystem::HandleHairColorChange(GamePlayer& player, int32_t newColor)
    {
        PlayerAppearanceComponent& appearanceComponent = player.GetAppearanceComponent();
        if (appearanceComponent.GetHairColor() == newColor)
        {
            return;
        }

        appearanceComponent.SetHairColor(newColor);

        Get<EntityViewRangeSystem>().Broadcast(player,
            GamePlayerMessageCreator::CreatePlayerHairColorChange(player, newColor), false);

        _serviceLocator.Get<GameRepositoryService>().SaveHairColor(player, newColor);
    }

    void PlayerAppearanceSystem::HandleHairChange(GamePlayer& player, int32_t newHair)
    {
        PlayerAppearanceComponent& appearanceComponent = player.GetAppearanceComponent();
        if (appearanceComponent.GetHair() == newHair)
        {
            return;
        }

        appearanceComponent.SetHair(newHair);
        const bool hasHat = appearanceComponent.GetHatModelId() != 0;

        Get<EntityViewRangeSystem>().Broadcast(player,
            GamePlayerMessageCreator::CreatePlayerHairChange(player, newHair, hasHat), false);

        _serviceLocator.Get<GameRepositoryService>().SaveHair(player, newHair);
    }

    auto PlayerAppearanceSystem::GetWeaponMotionCategory(const GamePlayer& player) const -> int32_t
    {
        const PlayerJobComponent& jobComponent = player.GetJobComponent();
        const Job& mainJob = jobComponent.GetMainJob();

        if (const GameItem* weapon = player.GetItemComponent().FindEquipmentItem(EquipmentPosition::Weapon1); weapon)
        {
            if (const sox::ItemWeapon* data = weapon->GetData().GetWeaponData(); data)
            {
                std::array<std::pair<int32_t, int32_t>, 3> list{
                    std::pair(data->jobMotion1, data->motionCategory1),
                    std::pair(data->jobMotion2, data->motionCategory2),
                    std::pair(data->jobMotion3, data->motionCategory3)
                };

                std::optional<int32_t> defaultMotionType = std::nullopt;

                for (const auto [job, category] : list)
                {
                    if (job == static_cast<int32_t>(JobId::Any))
                    {
                        defaultMotionType = category;
                    }

                    if (job == static_cast<int32_t>(mainJob.GetId()))
                    {
                        return category;
                    }
                }

                if (defaultMotionType.has_value())
                {
                    return *defaultMotionType;
                }
            }
            else
            {
                assert(false);
            }
        }

        if (mainJob.GetId() == JobId::MartialArtist)
        {
            return 5500;
        }

        return 1700;
    }
}
