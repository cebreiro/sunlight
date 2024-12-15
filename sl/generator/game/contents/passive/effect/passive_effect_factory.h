#pragma once

namespace sunlight
{
    struct SkillEffectData;

    class IPassiveEffect;
}

namespace sunlight
{
    class PassiveEffectFactory
    {
    public:
        PassiveEffectFactory() = delete;

        static auto CreateEffect(const SkillEffectData& data) -> std::unique_ptr<IPassiveEffect>;
    };
}
