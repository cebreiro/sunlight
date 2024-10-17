#include "passive_effect_poison_enchant.h"

namespace sunlight
{
    PassiveEffectPoisonEnchant::PassiveEffectPoisonEnchant(const SkillEffectData& data)
        : _data(data)
    {
    }

    auto PassiveEffectPoisonEnchant::GetType() const -> PassiveEffectType
    {
        return TYPE;
    }

    auto PassiveEffectPoisonEnchant::GetData() const -> const SkillEffectData&
    {
        return _data;
    }
}
