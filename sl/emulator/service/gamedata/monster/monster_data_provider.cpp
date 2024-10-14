#include "monster_data_provider.h"

#include "sl/emulator/game/data/sox_table_set.h"
#include "sl/emulator/game/data/sox/monster_action.h"
#include "sl/emulator/game/data/sox/monster_attack.h"
#include "sl/emulator/game/data/sox/monster_base.h"

namespace sunlight
{
    MonsterDataProvider::MonsterDataProvider(const SoxTableSet& tableSet)
    {
        const sox::MonsterBaseTable& soxMonsterBaseTable = tableSet.Get<sox::MonsterBaseTable>();
        const sox::MonsterActionTable& soxMonsterActionTable = tableSet.Get<sox::MonsterActionTable>();
        const sox::MonsterAttackTable& soxMonsterAttackTable = tableSet.Get<sox::MonsterAttackTable>();

        for (const sox::MonsterBase& soxBase : soxMonsterBaseTable.Get())
        {
            const sox::MonsterAction* soxAction = soxMonsterActionTable.Find(soxBase.index);
            if (!soxAction)
            {
                continue;
            }

            const sox::MonsterAttack* soxAttack = soxMonsterAttackTable.Find(soxBase.index);
            if (!soxAttack)
            {
                continue;
            }

            _monsterDataTable.try_emplace(soxBase.index, MonsterData(soxBase.index, soxBase, *soxAction, MonsterAttackData(*soxAttack)));
        }
    }

    auto MonsterDataProvider::GetName() const -> std::string_view
    {
        return "monster_data_provider";
    }

    auto MonsterDataProvider::Find(int32_t monsterId) const -> const MonsterData*
    {
        const auto iter = _monsterDataTable.find(monsterId);

        return iter != _monsterDataTable.end() ? &iter->second : nullptr;
    }
}
