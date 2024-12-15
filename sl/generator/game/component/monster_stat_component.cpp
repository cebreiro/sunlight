#include "monster_stat_component.h"

#include "sl/generator/game/data/sox/monster_base.h"

namespace sunlight
{
    MonsterStatComponent::MonsterStatComponent(const sox::MonsterBase& data)
        : _data(data)
        , _hp(data.hp)
    {
    }

    auto MonsterStatComponent::GetData() const -> const sox::MonsterBase&
    {
        return _data;
    }

    auto MonsterStatComponent::GetHP() const -> StatValue
    {
        return _hp;
    }

    void MonsterStatComponent::SetHP(StatValue value)
    {
        _hp = value;
    }
}
