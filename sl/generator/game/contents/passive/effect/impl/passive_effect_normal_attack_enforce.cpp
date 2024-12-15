#include "passive_effect_normal_attack_enforce.h"

#include "sl/generator/service/gamedata/skill/skill_effect_data.h"

namespace sunlight
{
    PassiveEffectNormalAttackEnforce::PassiveEffectNormalAttackEnforce(const SkillEffectData& data)
        : _data(data)
    {
    }

    auto PassiveEffectNormalAttackEnforce::GetType() const -> PassiveEffectType
    {
        return TYPE;
    }

    auto PassiveEffectNormalAttackEnforce::GetData() const -> const SkillEffectData&
    {
        return _data;
    }

    auto PassiveEffectNormalAttackEnforce::GetDamage(int32_t skillLevel) const -> int32_t
    {
        return _data.value2 + _data.value5 * skillLevel;
    }

    auto PassiveEffectNormalAttackEnforce::GetHitBonusRate() const -> float
    {
        return static_cast<float>(_data.value8) / 100.f;
    }
}
