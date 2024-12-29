#pragma once

namespace sunlight
{
    class EntityStatusEffectSystem;
}
namespace sunlight
{
    class PlayerSkillEffectStatusEffectHandlerRegister
    {
    public:
        PlayerSkillEffectStatusEffectHandlerRegister() = delete;

        static void Configure(EntityStatusEffectSystem& system);
    };
}
