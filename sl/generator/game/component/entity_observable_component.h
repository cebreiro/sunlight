#pragma once
#include "sl/generator/game/component/game_component.h"
#include "sl/generator/game/entity/game_entity_id_type.h"

namespace sunlight
{
    ENUM_CLASS(ObservableType, int32_t,
        (EntityPosition, 0)
    )

    class EntityObservableComponent final : public GameComponent
    {
    public:
        bool HasObserver(ObservableType type) const;
        bool HasObserver(ObservableType type, int64_t playerId) const;

        void AddObserver(ObservableType type, int64_t playerId);
        void RemoveObserver(ObservableType type, int64_t playerId);

        inline auto GetObserverRange(ObservableType type) const;

    private:
        std::unordered_multimap<ObservableType, int64_t> _observers;
    };

    inline auto EntityObservableComponent::GetObserverRange(ObservableType type) const
    {
        const auto [begin, end] = _observers.equal_range(type);

        return std::ranges::subrange(begin, end) | std::views::values;
    }
}
