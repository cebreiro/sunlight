#include "player_appearance_system.h"

#include "sl/emulator/game/component/player_appearance_component.h"
#include "sl/emulator/game/component/player_item_component.h"
#include "sl/emulator/game/component/player_job_component.h"
#include "sl/emulator/game/data/sox/item_weapon.h"
#include "sl/emulator/game/entity/game_item.h"
#include "sl/emulator/game/entity/game_player.h"
#include "sl/emulator/service/gamedata/item/item_data.h"

namespace sunlight
{
    PlayerAppearanceSystem::PlayerAppearanceSystem(const ServiceLocator& serviceLocator)
        : _serviceLocator(serviceLocator)
    {
    }

    void PlayerAppearanceSystem::InitializeSubSystem(Stage& stage)
    {
        (void)stage;
    }

    bool PlayerAppearanceSystem::Subscribe(Stage& stage)
    {
        (void)stage;

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

    void PlayerAppearanceSystem::RefreshWeaponMotionCategory(GamePlayer& player)
    {
        const int32_t weaponMotion = GetWeaponMotionCategory(player);

        player.GetAppearanceComponent().SetWeaponMotionCategory(weaponMotion);
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

                for (const auto [job, category] : list)
                {
                    if (job == static_cast<int32_t>(mainJob.GetId()))
                    {
                        return category;
                    }
                }
            }
            else
            {
                assert(false);
            }
        }

        // ITEM_BASE_ATTACK.SOX
        if (mainJob.GetId() == JobId::MartialArtist)
        {
            return 5500;
        }

        return 1700;
    }
}
