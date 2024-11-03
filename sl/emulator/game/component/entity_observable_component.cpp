#include "entity_observable_component.h"

namespace sunlight
{
    bool EntityObservableComponent::HasObserver(ObservableType type) const
    {
        return _observers.contains(type);
    }

    bool EntityObservableComponent::HasObserver(ObservableType type, int64_t playerId) const
    {
        const auto [begin, end] = _observers.equal_range(type);
        const auto iter = std::find_if(begin, end,
            [playerId](const auto& pair)
            {
                return pair.second == playerId;
            });

        return iter != end;
    }

    void EntityObservableComponent::AddObserver(ObservableType type, int64_t playerId)
    {
        _observers.emplace(type, playerId);
    }

    void EntityObservableComponent::RemoveObserver(ObservableType type, int64_t playerId)
    {
        const auto [begin, end] = _observers.equal_range(type);
        const auto iter = std::find_if(begin, end,
            [playerId](const auto& pair)
            {
                return pair.second == playerId;
            });

        assert(iter != end);
        _observers.erase(iter);
    }
}
