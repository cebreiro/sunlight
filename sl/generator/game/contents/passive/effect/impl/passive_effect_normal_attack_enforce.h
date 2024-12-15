#pragma once
#include "sl/generator/game/contents/passive/effect/passive_effect_interface.h"

namespace sunlight
{
    struct SkillEffectData;
}

namespace sunlight
{
    class PassiveEffectNormalAttackEnforce final : public IPassiveEffect
    {
    public:
        static constexpr auto TYPE = PassiveEffectType::NormalAttack;

    public:
        explicit PassiveEffectNormalAttackEnforce(const SkillEffectData& data);

        auto GetType() const -> PassiveEffectType override;
        auto GetData() const -> const SkillEffectData&;
        auto GetDamage(int32_t skillLevel) const -> int32_t;
        auto GetHitBonusRate() const -> float;

    private:
        const SkillEffectData& _data;
    };
}
