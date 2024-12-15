#include "monster_drop_item_table_system.h"

#include "sl/generator/game/game_constant.h"
#include "sl/generator/game/data/sox/item_etc.h"
#include "sl/generator/game/zone/service/zone_execution_service.h"
#include "sl/generator/service/gamedata/gamedata_provide_service.h"
#include "sl/generator/service/gamedata/item/item_data.h"
#include "sl/generator/service/gamedata/item/item_data_provider.h"
#include "sl/generator/service/gamedata/monster/monster_data.h"
#include "sl/generator/service/gamedata/monster/monster_drop_item_data.h"
#include "sl/generator/service/gamedata/monster/monster_drop_item_set_data.h"

namespace sunlight
{
    MonsterDropItemTableSystem::MonsterDropItemTableSystem(const ServiceLocator& serviceLocator, int32_t stageId)
        : _serviceLocator(serviceLocator)
        , _stageId(stageId)
        , _mt(std::random_device{}())
    {
        _goldItemData = _serviceLocator.Get<GameDataProvideService>().GetItemDataProvider().Find(GameConstant::ITEM_ID_GOLD);

        if (!_goldItemData)
        {
            assert(false);

            throw std::runtime_error("fail to find gold item data");
        }
    }

    void MonsterDropItemTableSystem::InitializeSubSystem(Stage& stage)
    {
        (void)stage;
    }

    bool MonsterDropItemTableSystem::Subscribe(Stage& stage)
    {
        (void)stage;

        return true;
    }

    auto MonsterDropItemTableSystem::GetName() const -> std::string_view
    {
        return "monster_drop_item_table_system";
    }

    auto MonsterDropItemTableSystem::GetClassId() const -> game_system_id_type
    {
        return GameSystem::GetClassId<MonsterDropItemTableSystem>();
    }

    void MonsterDropItemTableSystem::GetMonsterDropItem(std::vector<std::pair<PtrNotNull<const ItemData>, int32_t>>& result, const GameMonster& monster)
    {
        CreateDropItemTable(monster);

        std::uniform_int_distribution probabilityDist(0, GameConstant::ITEM_PROBABILITY_MAX);
        const MonsterDropItemData& dropItemData = monster.GetData().GetDropItemData();

        if (dropItemData.goldMod != 0 && probabilityDist(_mt) < dropItemData.goldMod)
        {
            result.emplace_back(_goldItemData,
                std::uniform_int_distribution{ dropItemData.goldMin, dropItemData.goldMax }(_mt));
        }

        const int32_t monsterId = monster.GetDataId();
        DropItemTable& dropItemTable = _dropItemTables[monsterId];

        for (int64_t i = 0; i < std::ssize(dropItemTable.itemLists); ++i)
        {
            const DropItemType dropItemType = static_cast<DropItemType>(i);
            auto& itemList = dropItemTable.itemLists[i];

            for (auto iter = itemList.begin(); iter != itemList.end(); )
            {
                const auto& [itemPtr, probability] = *iter;

                if (probability != GameConstant::ITEM_PROBABILITY_MAX && probabilityDist(_mt) >= probability)
                {
                    ++iter;

                    continue;
                }

                const int32_t quantity = RandItemQuantity(dropItemType, *itemPtr);

                result.emplace_back(itemPtr, quantity);

                if (itemPtr->GetWeaponData() || itemPtr->GetClothesData())
                {
                    const std::chrono::milliseconds genTime = RandItemGenTime(dropItemType, dropItemData.itemSetGenFrequency);

                    _serviceLocator.Get<ZoneExecutionService>().AddTimer(genTime,
                        [this, monsterId, i, itemPtr, probability]()
                        {
                            DropItemTable& dropItemTable = _dropItemTables[monsterId];

                            dropItemTable.itemLists[i].emplace_back(itemPtr, probability);
                        });

                    iter = itemList.erase(iter);
                }
                else
                {
                    ++iter;
                }
            }
        }

        for (int64_t i = 0; i < std::ssize(dropItemTable.itemSetLists); ++i)
        {
            const DropItemType dropItemType = static_cast<DropItemType>(i);

            std::vector<DropItemSet>& itemSetList = dropItemTable.itemSetLists[i];
            if (itemSetList.empty())
            {
                continue;
            }

            for (int64_t j = 0; j < std::ssize(itemSetList); ++j)
            {
                DropItemSet& itemSet = itemSetList[j];

                if (itemSet.items.empty())
                {
                    continue;
                }

                if (itemSet.probability != GameConstant::ITEM_PROBABILITY_MAX && probabilityDist(_mt) >= itemSet.probability)
                {
                    continue;
                }

                const int32_t value = std::uniform_int_distribution{ 0, itemSet.maxWeight }(_mt);
                int32_t sum = 0;

                for (auto iter = itemSet.items.begin(); iter != itemSet.items.end(); )
                {
                    const auto [itemPtr, weight] = *iter;
                    sum += weight;

                    if (value >= sum)
                    {
                        ++iter;

                        continue;
                    }

                    const int32_t quantity = RandItemQuantity(dropItemType, *itemPtr);

                    result.emplace_back(itemPtr, quantity);

                    if (itemPtr->GetWeaponData() || itemPtr->GetClothesData())
                    {
                        const std::chrono::milliseconds genTime = RandItemGenTime(dropItemType, dropItemData.itemSetGenFrequency);

                        _serviceLocator.Get<ZoneExecutionService>().AddTimer(genTime,
                            [this, monsterId, i, j, itemPtr, weight]()
                            {
                                DropItemTable& dropItemTable = _dropItemTables[monsterId];
                                DropItemSet& itemSet = dropItemTable.itemSetLists[i][j];

                                itemSet.maxWeight += weight;
                                itemSet.items.emplace_back(itemPtr, weight);
                            });

                        itemSet.maxWeight -= weight;
                        iter = itemSet.items.erase(iter);
                    }

                    break;
                }
            }
        }
    }

    void MonsterDropItemTableSystem::CreateDropItemTable(const GameMonster& monster)
    {
        const MonsterData& data = monster.GetData();

        const int32_t monsterId = data.GetId();
        if (_dropItemTables.contains(monsterId))
        {
            return;
        }

        DropItemTable& dropItemTableList = _dropItemTables[monsterId];
        const MonsterDropItemData& dropItemData = data.GetDropItemData();

        if (!dropItemData.items.empty())
        {
            auto& itemList = dropItemTableList.itemLists[static_cast<int32_t>(DropItemType::Normal)];
            itemList.reserve(dropItemData.items.size());

            for (const auto& [itemPtr, probability] : dropItemData.items)
            {
                itemList.emplace_back(itemPtr, probability);
            }
        }

        if (!dropItemData.rareItems.empty())
        {
            auto& itemList = dropItemTableList.itemLists[static_cast<int32_t>(DropItemType::Rare)];
            itemList.reserve(dropItemData.rareItems.size());

            for (const auto& [itemPtr, probability] : dropItemData.rareItems)
            {
                itemList.emplace_back(itemPtr, probability);
            }
        }

        if (!dropItemData.itemSets.empty())
        {
            auto& itemSetList = dropItemTableList.itemSetLists[static_cast<int32_t>(DropItemType::Normal)];
            itemSetList.reserve(dropItemData.itemSets.size());

            for (const auto& [itemSetData, itemSetProbability] : dropItemData.itemSets)
            {
                DropItemSet dropItemSet;
                dropItemSet.probability = itemSetProbability;
                dropItemSet.items.reserve(itemSetData->list.size());

                for (const auto& [itemPtr, itemProbability] : itemSetData->list)
                {
                    dropItemSet.items.emplace_back(itemPtr, itemProbability);
                    dropItemSet.maxWeight += itemProbability;
                }

                itemSetList.emplace_back(std::move(dropItemSet));
            }
        }

        if (!dropItemData.rareItemSets.empty())
        {
            auto& itemSetList = dropItemTableList.itemSetLists[static_cast<int32_t>(DropItemType::Rare)];
            itemSetList.reserve(dropItemData.rareItemSets.size());

            for (const auto& [itemSetData, itemSetProbability] : dropItemData.rareItemSets)
            {
                DropItemSet dropItemSet;
                dropItemSet.probability = itemSetProbability;
                dropItemSet.items.reserve(itemSetData->list.size());

                for (const auto& [itemPtr, itemProbability] : itemSetData->list)
                {
                    dropItemSet.items.emplace_back(itemPtr, itemProbability);
                    dropItemSet.maxWeight += itemProbability;
                }

                itemSetList.emplace_back(std::move(dropItemSet));
            }
        }
    }

    auto MonsterDropItemTableSystem::RandItemQuantity(DropItemType type, const ItemData& itemData) -> int32_t
    {
        const int32_t maxOverlapCount = itemData.GetMaxOverlapCount();
        if (maxOverlapCount <= 1)
        {
            return 1;
        }

        bool isBullet = itemData.GetEtcData() ? itemData.GetEtcData()->bulletType : false;

        const auto minmax = [&]() -> std::pair<int32_t, int32_t>
            {
                if (isBullet)
                {
                    return std::make_pair(30, 200);
                }

                if (const sox::ItemWeapon* weaponData = itemData.GetWeaponData(); weaponData)
                {
                    if (type == DropItemType::Rare)
                    {
                        return std::make_pair(maxOverlapCount * 2 / 3, maxOverlapCount);
                    }

                    return std::make_pair(50, 300);
                }

                return std::make_pair(1, 1);
            }();

        const int32_t min = std::min(minmax.first, maxOverlapCount);
        const int32_t max = std::min(minmax.second, maxOverlapCount);

        if (min == max)
        {
            return min;
        }

        return std::uniform_int_distribution{ min, max }(_mt);
    }

    auto MonsterDropItemTableSystem::RandItemGenTime(DropItemType type, int32_t frequency) -> std::chrono::milliseconds
    {
        switch (type)
        {
        case DropItemType::Normal:
        {
            const int32_t value =
                (180 + std::uniform_int_distribution{ -60, 60 }(_mt))
                / std::max(1, frequency);

            return std::chrono::seconds(value);
        }
        break;
        case DropItemType::Rare:
        {
            const int32_t value =
                (60 + std::uniform_int_distribution{ -10, 10 }(_mt))
                / std::max(1, frequency);

            return std::chrono::minutes(value);
        }
        default:
            assert(false);
        }

        return std::chrono::hours(1);
    }
}
