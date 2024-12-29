#include "monster_data_provider.h"

#include "sl/generator/game/data/sox_table_set.h"
#include "sl/generator/game/data/sox/item_set.h"
#include "sl/generator/game/data/sox/monster_action.h"
#include "sl/generator/game/data/sox/monster_attack.h"
#include "sl/generator/game/data/sox/monster_base.h"
#include "sl/generator/game/data/sox/monster_item.h"
#include "sl/generator/service/gamedata/abf/abf_data_provider.h"
#include "sl/generator/service/gamedata/item/item_data_provider.h"

namespace sunlight
{
    MonsterDataProvider::MonsterDataProvider(const SoxTableSet& tableSet, const ItemDataProvider& itemDataProvider)
    {
        for (const sox::ItemSet& soxItemSet : tableSet.Get<sox::ItemSetTable>().Get())
        {
            const std::initializer_list<std::pair<int32_t, int32_t>> list{
                { soxItemSet.item01, soxItemSet.item01Mod },
                { soxItemSet.item02, soxItemSet.item02Mod },
                { soxItemSet.item03, soxItemSet.item03Mod },
                { soxItemSet.item04, soxItemSet.item04Mod },
                { soxItemSet.item05, soxItemSet.item05Mod },
                { soxItemSet.item06, soxItemSet.item06Mod },
                { soxItemSet.item07, soxItemSet.item07Mod },
                { soxItemSet.item08, soxItemSet.item08Mod },
                { soxItemSet.item09, soxItemSet.item09Mod },
                { soxItemSet.item10, soxItemSet.item10Mod },
                { soxItemSet.item11, soxItemSet.item11Mod },
                { soxItemSet.item11, soxItemSet.item11Mod },
                { soxItemSet.item12, soxItemSet.item12Mod },
                { soxItemSet.item13, soxItemSet.item13Mod },
                { soxItemSet.item14, soxItemSet.item14Mod },
                { soxItemSet.item15, soxItemSet.item15Mod },
                { soxItemSet.item16, soxItemSet.item16Mod },
                { soxItemSet.item17, soxItemSet.item17Mod },
                { soxItemSet.item18, soxItemSet.item18Mod },
                { soxItemSet.item19, soxItemSet.item19Mod },
                { soxItemSet.item20, soxItemSet.item20Mod },
                { soxItemSet.item21, soxItemSet.item21Mod },
                { soxItemSet.item22, soxItemSet.item22Mod },
                { soxItemSet.item23, soxItemSet.item23Mod },
                { soxItemSet.item24, soxItemSet.item24Mod },
                { soxItemSet.item25, soxItemSet.item25Mod },
                { soxItemSet.item26, soxItemSet.item26Mod },
                { soxItemSet.item27, soxItemSet.item27Mod },
                { soxItemSet.item28, soxItemSet.item28Mod },
                { soxItemSet.item29, soxItemSet.item29Mod },
                { soxItemSet.item30, soxItemSet.item30Mod },
                { soxItemSet.item31, soxItemSet.item31Mod },
                { soxItemSet.item32, soxItemSet.item32Mod },
                { soxItemSet.item33, soxItemSet.item33Mod },
                { soxItemSet.item34, soxItemSet.item34Mod },
                { soxItemSet.item35, soxItemSet.item35Mod },
                { soxItemSet.item36, soxItemSet.item36Mod },
                { soxItemSet.item37, soxItemSet.item37Mod },
                { soxItemSet.item38, soxItemSet.item38Mod },
                { soxItemSet.item39, soxItemSet.item39Mod },
                { soxItemSet.item40, soxItemSet.item40Mod },
                { soxItemSet.item41, soxItemSet.item41Mod },
                { soxItemSet.item42, soxItemSet.item42Mod },
                { soxItemSet.item43, soxItemSet.item43Mod },
                { soxItemSet.item44, soxItemSet.item44Mod },
                { soxItemSet.item45, soxItemSet.item45Mod },
                { soxItemSet.item46, soxItemSet.item46Mod },
                { soxItemSet.item47, soxItemSet.item47Mod },
                { soxItemSet.item48, soxItemSet.item48Mod },
                { soxItemSet.item49, soxItemSet.item49Mod },
                { soxItemSet.item50, soxItemSet.item50Mod },
            };

            MonsterDropItemSetData* dropItemSetData = nullptr;

           for (const auto [itemId, probability] : list)
           {
               if (itemId == 0 || probability <= 0)
               {
                   continue;
               }

               const ItemData* itemData = itemDataProvider.Find(itemId);
               if (!itemData)
               {
                   continue;
               }

               if (!dropItemSetData)
               {
                   dropItemSetData = &_monsterDropItemSetTable[soxItemSet.index];
                   dropItemSetData->id = soxItemSet.index;
               }

               dropItemSetData->list.emplace_back(itemData, probability);
           }
        }

        const sox::MonsterBaseTable& soxMonsterBaseTable = tableSet.Get<sox::MonsterBaseTable>();
        const sox::MonsterActionTable& soxMonsterActionTable = tableSet.Get<sox::MonsterActionTable>();
        const sox::MonsterAttackTable& soxMonsterAttackTable = tableSet.Get<sox::MonsterAttackTable>();
        const sox::MonsterItemTable& soxMonsterItemTable = tableSet.Get<sox::MonsterItemTable>();

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

            MonsterDropItemData dropItemData;

            if (const sox::MonsterItem* soxMonsterItem = soxMonsterItemTable.Find(soxBase.index);
                soxMonsterItem)
            {
                dropItemData = MonsterDropItemData(*soxMonsterItem, itemDataProvider, _monsterDropItemSetTable);
            }

            _monsterDataTable.try_emplace(soxBase.index, MonsterData(soxBase.index, soxBase, *soxAction, MonsterAttackData(*soxAttack), std::move(dropItemData)));
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
