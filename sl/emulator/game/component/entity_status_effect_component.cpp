#include "entity_status_effect_component.h"

namespace sunlight
{
    bool EntityStatusEffectComponent::Contains(int32_t statusEffectId) const
    {
        return _statusEffects.contains(statusEffectId);
    }

    bool EntityStatusEffectComponent::Add(const StatusEffect& statusEffect)
    {
        return _statusEffects.try_emplace(statusEffect.GetId(), statusEffect).second;
    }

    bool EntityStatusEffectComponent::Remove(int32_t statusEffectId)
    {
        return _statusEffects.erase(statusEffectId) > 0;
    }

    auto EntityStatusEffectComponent::GetCount() const -> int64_t
    {
        return std::ssize(_statusEffects);
    }
}
