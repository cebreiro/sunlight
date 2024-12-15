#pragma once
#include "sl/generator/service/gamedata/skill/status_effect_type.h"

namespace sunlight
{
    struct PlayerSkillEffectStatusEffectHandlerParam;
    class StatusEffect;

    class GameEntity;

    class EntityStatusEffectSystem;
}

namespace sunlight
{
    class IStatusEffectHandler
    {
    public:
        virtual ~IStatusEffectHandler() = default;

        virtual auto GetType() const -> StatusEffectType = 0;
    };

    class IStatusEffectApplyHandler
    {
    public:
        virtual ~IStatusEffectApplyHandler() = default;

        virtual void Apply(EntityStatusEffectSystem& system, GameEntity& entity, StatusEffect& statusEffect) const = 0;
    };

    class IStatusEffectRevertHandler
    {
    public:
        virtual ~IStatusEffectRevertHandler() = default;

        virtual void Revert(EntityStatusEffectSystem& system, GameEntity& entity, StatusEffect& statusEffect) const = 0;
    };

    class IStatusEffectTickHandler
    {
    public:
        virtual ~IStatusEffectTickHandler() = default;

        virtual void Tick(EntityStatusEffectSystem& system, GameEntity& entity, StatusEffect& statusEffect) const = 0;
    };
}
