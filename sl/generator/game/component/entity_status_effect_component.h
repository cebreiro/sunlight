#pragma once
#include "sl/generator/game/component/game_component.h"
#include "sl/generator/game/contents/status_effect/status_effect.h"

namespace sunlight
{
    class EntityStatusEffectComponent final : public GameComponent
    {
    public:
        bool Contains(int32_t statusEffectId) const;

        auto Add(const StatusEffect& statusEffect) -> StatusEffect*;
        bool Remove(int32_t statusEffectId);

        auto Release(int32_t statusEffectId) -> std::optional<StatusEffect>;

        auto Find(int32_t id) -> StatusEffect*;
        auto Find(int32_t id) const -> const StatusEffect*;

        auto GetCount() const -> int64_t;
        inline auto GetRange();
        inline auto GetRange() const;

    private:
        std::unordered_map<int32_t, StatusEffect> _statusEffects;
    };

    auto EntityStatusEffectComponent::GetRange()
    {
        return _statusEffects | std::views::values;
    }

    auto EntityStatusEffectComponent::GetRange() const
    {
        return _statusEffects | std::views::values;
    }
}
