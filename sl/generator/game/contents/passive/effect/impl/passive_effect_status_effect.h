#pragma once
#include "sl/generator/game/contents/passive/effect/passive_effect_interface.h"

namespace sunlight
{
    struct SkillEffectData;
}

namespace sunlight
{
    class PassiveEffectStatusEffect final : public IPassiveEffect
    {
    public:
        static constexpr auto TYPE = PassiveEffectType::StatusEffect;

        explicit PassiveEffectStatusEffect(const SkillEffectData& data);

        auto GetType() const -> PassiveEffectType override;

    public:
        auto GetData() const -> const SkillEffectData&;

    private:
        const SkillEffectData& _data;
    };
}
