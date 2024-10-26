#include "passive_effect_status_effect.h"

namespace sunlight
{
    PassiveEffectStatusEffect::PassiveEffectStatusEffect(const SkillEffectData& data)
        : _data(data)
    {
    }

    auto PassiveEffectStatusEffect::GetType() const -> PassiveEffectType
    {
        return TYPE;
    }

    auto PassiveEffectStatusEffect::GetData() const -> const SkillEffectData&
    {
        return _data;
    }
}
