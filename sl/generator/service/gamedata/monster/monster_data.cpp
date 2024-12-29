#include "monster_data.h"

namespace sunlight
{
    MonsterData::MonsterData(int32_t id, const sox::MonsterBase& base, const sox::MonsterAction& action, const MonsterAttackData& attack, MonsterDropItemData dropItemData)
        : _id(id)
        , _base(&base)
        , _action(&action)
        , _attack(attack)
        , _dropItemData(std::move(dropItemData))
    {
    }

    auto MonsterData::GetId() const -> int32_t
    {
        return _id;
    }

    auto MonsterData::GetBase() const -> const sox::MonsterBase&
    {
        assert(_base);

        return *_base;
    }

    auto MonsterData::GetAction() const -> const sox::MonsterAction&
    {
        assert(_action);

        return *_action;
    }

    auto MonsterData::GetAttack() const -> const MonsterAttackData&
    {
        return _attack;
    }

    auto MonsterData::GetDropItemData() const -> const MonsterDropItemData&
    {
        return _dropItemData;
    }
}
