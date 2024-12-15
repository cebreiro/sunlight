#include "entity_passive_effect_component.h"

namespace sunlight
{
    void EntityPassiveEffectComponent::AddEffect(int32_t skillId, PtrNotNull<IPassiveEffect> effect)
    {
        _effects.emplace(skillId, effect);
        _effectTypeIndex[static_cast<int64_t>(effect->GetType())].push_back(effect);
    }

    void EntityPassiveEffectComponent::RemoveEffects(int32_t skillId)
    {
        const auto [begin, end] = _effects.equal_range(skillId);

        for (auto iter = begin; iter != end; ++iter)
        {
            PtrNotNull<IPassiveEffect> passiveEffect = iter->second;

            std::vector<PtrNotNull<IPassiveEffect>>& list = _effectTypeIndex[static_cast<int64_t>(passiveEffect->GetType())];

            const auto iter2 = std::ranges::find(list, passiveEffect);
            assert(iter2 != list.end());

            list.erase(iter2);
        }

        _effects.erase(skillId);
    }
}
