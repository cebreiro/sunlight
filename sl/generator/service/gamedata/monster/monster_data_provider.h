#pragma once
#include "sl/generator/service/gamedata/monster/monster_data.h"
#include "sl/generator/service/gamedata/monster/monster_drop_item_set_data.h"

namespace sunlight
{
    class SoxTableSet;

    class ItemDataProvider;
    class AbilityFileDataProvider;
}

namespace sunlight
{
    class MonsterDataProvider
    {
    public:
        MonsterDataProvider(const SoxTableSet& tableSet, const ItemDataProvider& itemDataProvider);

        auto GetName() const -> std::string_view;

    public:
        auto Find(int32_t monsterId) const -> const MonsterData*;

    private:
        std::unordered_map<int32_t, MonsterDropItemSetData> _monsterDropItemSetTable;
        std::unordered_map<int32_t, MonsterData> _monsterDataTable;
    };
}
