#include "status_effect_stat_increase.h"

namespace sunlight
{
    auto StatusEffectStatIncreaseHandler::GetType() const -> StatusEffectType
    {
        return StatusEffectType::StatIncrease;
    }

    void StatusEffectStatIncreaseHandler::Apply(GameEntity& entity, StatusEffect& statusEffect) const
    {
        (void)entity;
        (void)statusEffect;
    }

    void StatusEffectStatIncreaseHandler::Revert(GameEntity& entity, StatusEffect& statusEffect) const
    {
        (void)entity;
        (void)statusEffect;
    }
}
