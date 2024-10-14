#pragma once
#include "sl/emulator/service/gamedata/monster/monster_data.h"

namespace sunlight
{
    class SoxTableSet;
}

namespace sunlight
{
    class MonsterDataProvider
    {
    public:
        explicit MonsterDataProvider(const SoxTableSet& tableSet);

        auto GetName() const -> std::string_view;

    public:
        auto Find(int32_t monsterId) const -> const MonsterData*;

    private:
        std::unordered_map<int32_t, MonsterData> _monsterDataTable;
    };
}
