#include "monster_drop_item_data.h"

#include "sl/generator/game/data/sox/monster_item.h"
#include "sl/generator/service/gamedata/item/item_data_provider.h"
#include "sl/generator/service/gamedata/monster/monster_drop_item_set_data.h"

namespace sunlight
{
    MonsterDropItemData::MonsterDropItemData(const sox::MonsterItem& data, const ItemDataProvider& itemDataProvider,
        const std::unordered_map<int32_t, MonsterDropItemSetData>& itemSetTable)
        : itemGenFrequency(data.itemGenFrequency)
        , itemSetGenFrequency(data.itemsetGenFrequency)
        , rareItemGenFrequency(data.rareItemGenFrequency)
        , rareItemSetGenFrequency(data.rareItemsetGenFrequency)
        , goldMod(data.itemGoldMod)
        , goldMin(data.itemGoldMin)
        , goldMax(data.itemGoldMax)
    {
        for (const auto& [itemId, probability] : std::initializer_list<std::pair<int32_t, int32_t>>{
            { data.item01, data.item01Mod },
            { data.item02, data.item02Mod },
            { data.item03, data.item03Mod },
            { data.item04, data.item04Mod },
            { data.item05, data.item05Mod },
            { data.item06, data.item06Mod },
            { data.item07, data.item07Mod },
            { data.item08, data.item08Mod },
            { data.item09, data.item09Mod },
            { data.item10, data.item10Mod },
            { data.item11, data.item11Mod },
            { data.item11, data.item11Mod },
            { data.item12, data.item12Mod },
            { data.item13, data.item13Mod },
            { data.item14, data.item14Mod },
            { data.item15, data.item15Mod },
            { data.item16, data.item16Mod },
            { data.item17, data.item17Mod },
            { data.item18, data.item18Mod },
            { data.item19, data.item19Mod },
            { data.item20, data.item20Mod },
            { data.item21, data.item21Mod },
            { data.item22, data.item22Mod },
            { data.item23, data.item23Mod },
            { data.item24, data.item24Mod },
            { data.item25, data.item25Mod },
            { data.item26, data.item26Mod },
            { data.item27, data.item27Mod },
            { data.item28, data.item28Mod },
            { data.item29, data.item29Mod },
            { data.item30, data.item30Mod }
        })
        {
            if (itemId == 0 || probability == 0)
            {
                continue;
            }

            const ItemData* itemData = itemDataProvider.Find(itemId);
            if (!itemData)
            {
                continue;
            }

            items.emplace_back(itemData, probability);
        }

        for (const auto& [itemId, probability] : std::initializer_list<std::pair<int32_t, int32_t>>{
            { data.rareItem01, data.rareItem01Mod },
            { data.rareItem02, data.rareItem02Mod },
            { data.rareItem03, data.rareItem03Mod },
            { data.rareItem04, data.rareItem04Mod },
            { data.rareItem05, data.rareItem05Mod },
            { data.rareItem06, data.rareItem06Mod },
            { data.rareItem07, data.rareItem07Mod },
            { data.rareItem08, data.rareItem08Mod },
            { data.rareItem09, data.rareItem09Mod },
            { data.rareItem10, data.rareItem10Mod },
        })
        {
            if (itemId == 0 || probability == 0)
            {
                continue;
            }

            const ItemData* itemData = itemDataProvider.Find(itemId);
            if (!itemData)
            {
                continue;
            }

            rareItems.emplace_back(itemData, probability);
        }

        for (const auto& [itemSetId, probability] : std::initializer_list<std::pair<int32_t, int32_t>>{
            { data.itemset01, data.itemset01Mod },
            { data.itemset02, data.itemset02Mod },
            { data.itemset03, data.itemset03Mod },
            { data.itemset04, data.itemset04Mod },
            { data.itemset05, data.itemset05Mod },
            { data.itemset06, data.itemset06Mod },
            { data.itemset07, data.itemset07Mod },
            { data.itemset08, data.itemset08Mod },
            { data.itemset09, data.itemset09Mod },
            { data.itemset10, data.itemset10Mod },
        })
        {
            if (itemSetId == 0 || probability == 0)
            {
                continue;
            }

            const auto iter = itemSetTable.find(itemSetId);
            if (iter == itemSetTable.end())
            {
                continue;
            }

            const MonsterDropItemSetData& itemSetData = iter->second;

            itemSets.emplace_back(&itemSetData, probability);
        }

        for (const auto& [itemSetId, probability] : std::initializer_list<std::pair<int32_t, int32_t>>{
            { data.rareItemset01, data.rareItemset01Mod },
            { data.rareItemset02, data.rareItemset02Mod },
            { data.rareItemset03, data.rareItemset03Mod },
            { data.rareItemset04, data.rareItemset04Mod },
            { data.rareItemset05, data.rareItemset05Mod },
            { data.rareItemset06, data.rareItemset06Mod },
            { data.rareItemset07, data.rareItemset07Mod },
            { data.rareItemset08, data.rareItemset08Mod },
            { data.rareItemset09, data.rareItemset09Mod },
            { data.rareItemset10, data.rareItemset10Mod },
        })
        {
            if (itemSetId == 0 || probability == 0)
            {
                continue;
            }

            const auto iter = itemSetTable.find(itemSetId);
            if (iter == itemSetTable.end())
            {
                continue;
            }

            const MonsterDropItemSetData& itemSetData = iter->second;

            rareItemSets.emplace_back(&itemSetData, probability);
        }

        items.shrink_to_fit();
        rareItems.shrink_to_fit();
        itemSets.shrink_to_fit();
        rareItemSets.shrink_to_fit();
    }
}
