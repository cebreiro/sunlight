#pragma once
#include "sl/emulator/game/contents/passive/effect/passive_effect_interface.h"

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

    private:
        const SkillEffectData& _data;
    };
}
