#include "entity_immune_component.h"

namespace sunlight
{
    bool EntityImmuneComponent::Contains(ImmuneType immuneType) const
    {
        const auto iter = _immuneTypes.find(immuneType);

        return iter != _immuneTypes.end();
    }

    void EntityImmuneComponent::Add(const ImmuneOrigin& origin)
    {
        _immuneTypes.emplace(origin.GetImmuneType(), origin);
    }

    bool EntityImmuneComponent::Remove(const ImmuneOrigin& origin)
    {
        const auto [begin, end] = _immuneTypes.equal_range(origin.GetImmuneType());

        for (auto iter = begin; iter != end; ++iter)
        {
            if (iter->second == origin)
            {
                _immuneTypes.erase(iter);

                return true;
            }
        }

        return false;
    }
}
