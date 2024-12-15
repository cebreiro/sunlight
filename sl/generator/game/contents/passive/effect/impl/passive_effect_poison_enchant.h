#pragma once
#include "sl/generator/game/contents/passive/effect/passive_effect_interface.h"

namespace sunlight
{
    struct SkillEffectData;
}


namespace sunlight
{
    class PassiveEffectPoisonEnchant final : public IPassiveEffect
    {
    public:
        static constexpr auto TYPE = PassiveEffectType::PoisonEnchant;
    
    public:
        explicit PassiveEffectPoisonEnchant(const SkillEffectData& data);

        auto GetType() const -> PassiveEffectType override;
        auto GetData() const -> const SkillEffectData&;

    private:
        const SkillEffectData& _data;
    };
}
