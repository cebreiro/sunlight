#pragma once
#include "sl/emulator/service/gamedata/skill/status_effect_type.h"

namespace sunlight
{
    struct PlayerSkillEffectStatusEffectHandlerParam;
    class StatusEffect;

    class GameEntity;
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

        virtual void Apply(GameEntity& entity, const StatusEffect& statusEffect) const = 0;
    };

    class IStatusEffectRevertHandler
    {
    public:
        virtual ~IStatusEffectRevertHandler() = default;

        virtual void Revert(GameEntity& entity, const StatusEffect& statusEffect) const = 0;
    };

    class IStatusEffectTickHandler
    {
    public:
        virtual ~IStatusEffectTickHandler() = default;

        virtual void Tick(GameEntity& entity, const StatusEffect& statusEffect) const = 0;
    };
}
