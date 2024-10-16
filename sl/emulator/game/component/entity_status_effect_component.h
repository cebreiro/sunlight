#pragma once
#include "sl/emulator/game/component/game_component.h"
#include "sl/emulator/game/contents/status_effect/status_effect.h"

namespace sunlight
{
    class EntityStatusEffectComponent final : public GameComponent
    {
    public:
        bool Contains(int32_t statusEffectId) const;

        bool Add(const StatusEffect& statusEffect);
        bool Remove(int32_t statusEffectId);

        auto GetCount() const -> int64_t;
        inline auto GetRange() const;

    private:
        std::unordered_map<int32_t, StatusEffect> _statusEffects;
    };

    auto EntityStatusEffectComponent::GetRange() const
    {
        return _statusEffects | std::views::values;
    }
}
