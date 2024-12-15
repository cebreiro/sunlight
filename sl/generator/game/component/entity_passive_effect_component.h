#pragma once
#include "sl/generator/game/component/game_component.h"
#include "sl/generator/game/contents/passive/effect/passive_effect_interface.h"

namespace sunlight
{
    class EntityPassiveEffectComponent final : public GameComponent
    {
    public:
        void AddEffect(int32_t skillId, PtrNotNull<IPassiveEffect> effect);
        void RemoveEffects(int32_t skillId);

        template <typename T> requires std::derived_from<T, IPassiveEffect>
        auto Find() -> T*;

        template <typename T> requires std::derived_from<T, IPassiveEffect>
        auto Find() const -> const T*;

    private:
        std::unordered_multimap<int32_t, PtrNotNull<IPassiveEffect>> _effects;
        std::array<std::vector<PtrNotNull<IPassiveEffect>>, PassiveEffectTypeMeta::GetSize()> _effectTypeIndex;
    };

    template <typename T> requires std::derived_from<T, IPassiveEffect>
    auto EntityPassiveEffectComponent::Find() -> T*
    {
        const int64_t index = static_cast<int64_t>(T::TYPE);
        assert(index >= 0 && index < (std::ssize(_effectTypeIndex)));

        const std::vector<PtrNotNull<IPassiveEffect>>& list = _effectTypeIndex[index];

        return list.empty() ? nullptr : Cast<T>(*list[0]);
    }

    template <typename T> requires std::derived_from<T, IPassiveEffect>
    auto EntityPassiveEffectComponent::Find() const -> const T*
    {
        const int64_t index = static_cast<int64_t>(T::TYPE);
        assert(index >= 0 && index < (std::ssize(_effectTypeIndex)));

        const std::vector<PtrNotNull<IPassiveEffect>>& list = _effectTypeIndex[index];
        
        return list.empty() ? nullptr : Cast<T>(*list[0]);
    }
}
