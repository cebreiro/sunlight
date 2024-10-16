#pragma once
#include "sl/emulator/game/contents/status_effect/status_effect_handler_interface.h"

namespace sunlight
{
    class StatusEffectStatIncreaseHandler final
        : public IStatusEffectHandler
        , public IStatusEffectApplyHandler
        , public IStatusEffectRevertHandler
    {
    public:
        auto GetType() const -> StatusEffectType override;

        void Apply(GameEntity& entity, StatusEffect& statusEffect) const override;
        void Revert(GameEntity& entity, StatusEffect& statusEffect) const override;
    };
}
