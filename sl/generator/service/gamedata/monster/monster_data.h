#pragma once
#include "sl/generator/service/gamedata/monster/monster_attack_data.h"
#include "sl/generator/service/gamedata/monster/monster_drop_item_data.h"

namespace sunlight::sox
{
    struct MonsterBase;
    struct MonsterAction;
}

namespace sunlight
{
    struct MonsterDropItemData;
}

namespace sunlight
{
    class MonsterData
    {
    public:
        MonsterData() = default;
        MonsterData(int32_t id, const sox::MonsterBase& base, const sox::MonsterAction& action,
            const MonsterAttackData& attack, MonsterDropItemData dropItemData);

        auto GetId() const -> int32_t;
        auto GetBase() const -> const sox::MonsterBase&;
        auto GetAction() const -> const sox::MonsterAction&;
        auto GetAttack() const -> const MonsterAttackData&;
        auto GetDropItemData() const -> const MonsterDropItemData&;

    private:
        int32_t _id = 0;

        PtrNotNull<const sox::MonsterBase> _base = nullptr;
        PtrNotNull<const sox::MonsterAction> _action = nullptr;
        MonsterAttackData _attack = {};
        MonsterDropItemData _dropItemData;
    };
}
