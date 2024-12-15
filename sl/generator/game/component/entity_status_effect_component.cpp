#include "entity_status_effect_component.h"

namespace sunlight
{
    bool EntityStatusEffectComponent::Contains(int32_t statusEffectId) const
    {
        return _statusEffects.contains(statusEffectId);
    }

    auto EntityStatusEffectComponent::Add(const StatusEffect& statusEffect) -> StatusEffect*
    {
        const auto [iter, inserted] = _statusEffects.try_emplace(statusEffect.GetId(), statusEffect);
        if (!inserted)
        {
            return nullptr;
        }

        return &iter->second;
    }

    bool EntityStatusEffectComponent::Remove(int32_t statusEffectId)
    {
        return _statusEffects.erase(statusEffectId) > 0;
    }

    auto EntityStatusEffectComponent::Release(int32_t statusEffectId) -> std::optional<StatusEffect>
    {
        const auto iter = _statusEffects.find(statusEffectId);
        if (iter == _statusEffects.end())
        {
            return std::nullopt;
        }

        StatusEffect result = std::move(iter->second);
        _statusEffects.erase(iter);

        return result;
    }

    auto EntityStatusEffectComponent::Find(int32_t id) -> StatusEffect*
    {
        const auto iter = _statusEffects.find(id);

        return iter != _statusEffects.end() ? &iter->second : nullptr;
    }

    auto EntityStatusEffectComponent::Find(int32_t id) const -> const StatusEffect*
    {
        const auto iter = _statusEffects.find(id);

        return iter != _statusEffects.end() ? &iter->second : nullptr;
    }

    auto EntityStatusEffectComponent::GetCount() const -> int64_t
    {
        return std::ssize(_statusEffects);
    }
}
