#include "player_motion_data_provider.h"

#include "sl/generator/game/data/sox_table_set.h"
#include "sl/generator/game/data/sox/motion_data.h"
#include "sl/generator/game/data/sox/weapon_motion_type.h"

namespace sunlight
{
    PlayerMotionDataProvider::PlayerMotionDataProvider(const SoxTableSet& tableSet)
    {
        const sox::MotionDataTable& soxMotionDataTable = tableSet.Get<sox::MotionDataTable>();

        for (const sox::WeaponMotionType& soxWeaponMotion : tableSet.Get<sox::WeaponMotionTypeTable>().Get())
        {
            const std::initializer_list<int32_t> motions{
                soxWeaponMotion.attackMotion1,
                soxWeaponMotion.attackMotion2,
                soxWeaponMotion.attackMotion3,
                soxWeaponMotion.attackMotion4,
                soxWeaponMotion.attackMotion5,
                soxWeaponMotion.attackMotion6,
                soxWeaponMotion.attackMotion7,
                soxWeaponMotion.attackMotion8,
                soxWeaponMotion.attackMotion9,
                soxWeaponMotion.attackMotion10,
            };

            std::vector<const sox::MotionData*>& normalAttackMotions = _normalAttackMotions[soxWeaponMotion.index];
            normalAttackMotions.reserve(soxWeaponMotion.maxCombo);

            for (int32_t attackMotion : motions)
            {
                if (attackMotion == 0)
                {
                    break;
                }

                const sox::MotionData* data = soxMotionDataTable.Find(attackMotion);
                if (!data)
                {
                    break;
                }

                normalAttackMotions.push_back(data);
            }
        }
    }

    auto PlayerMotionDataProvider::FindNormalAttackMotion(int32_t weaponClass, int32_t combo) const -> const sox::MotionData*
    {
        const auto iter = _normalAttackMotions.find(weaponClass);
        if (iter == _normalAttackMotions.end())
        {
            return nullptr;
        }

        const std::vector<const sox::MotionData*>& table = iter->second;
        if (combo < 0 || combo >= std::ssize(table))
        {
            return nullptr;
        }

        return table[combo];
    }
}
