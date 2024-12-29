#pragma once
#include "sl/generator/game/contents/passive/effect/passive_effect_type.h"

namespace sunlight
{
    class IPassiveEffect
    {
    public:
        virtual ~IPassiveEffect() = default;

        virtual auto GetType() const -> PassiveEffectType = 0;
    };

    template <typename T> requires std::derived_from<T, IPassiveEffect>
    auto Cast(IPassiveEffect& passiveEffect) -> T*
    {
        if (passiveEffect.GetType() != T::TYPE)
        {
            return nullptr;
        }

        T* result = static_cast<T*>(&passiveEffect);
        assert(dynamic_cast<T*>(&passiveEffect));

        return result;
    }

    template <typename T> requires std::derived_from<T, IPassiveEffect>
    auto Cast(const IPassiveEffect& passiveEffect) -> const T*
    {
        if (passiveEffect.GetType() != T::TYPE)
        {
            return nullptr;
        }

        const T* result = static_cast<const T*>(&passiveEffect);
        assert(dynamic_cast<const T*>(&passiveEffect));

        return result;
    }
}
