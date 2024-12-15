#pragma once
#include "sl/generator/game/contents/passive/effect/passive_effect_interface.h"

namespace sunlight
{
    struct SkillEffectData;
}

namespace sunlight
{
    class PassiveEffectStat final : public IPassiveEffect
    {
    public:
        static constexpr auto TYPE = PassiveEffectType::Stat;

        explicit PassiveEffectStat(const SkillEffectData& data);

        auto GetType() const -> PassiveEffectType override;
        auto GetData() const -> const SkillEffectData&;
        auto GetStatType() const -> int32_t;

        auto GetStatValue() const -> int32_t;
        auto GetStatPercentageValue() const -> double;

        void SetStatValue(int32_t value);
        void SetStatPercentageValue(double value);

    private:
        const SkillEffectData& _data;

        int32_t _statValue = 0;
        double _statPercentageValue = 0.0;
    };
}
