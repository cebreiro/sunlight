#include "monster_stat_component.h"

#include "sl/emulator/game/data/sox/monster_base.h"

namespace sunlight
{
    MonsterStatComponent::MonsterStatComponent(const sox::MonsterBase& data)
        : _data(data)
        , _hp(data.hp)
    {
    }

    bool MonsterStatComponent::IsDead() const
    {
        return _dead;
    }

    auto MonsterStatComponent::GetData() const -> const sox::MonsterBase&
    {
        return _data;
    }

    auto MonsterStatComponent::GetHP() const -> StatValue
    {
        return _hp;
    }

    void MonsterStatComponent::SetDead(bool value)
    {
        _dead = value;
    }

    void MonsterStatComponent::SetHP(StatValue value)
    {
        _hp = value;
    }
}
