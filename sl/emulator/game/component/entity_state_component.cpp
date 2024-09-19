#include "entity_state_component.h"

namespace sunlight
{
    auto EntityStateComponent::GetState() const -> const GameEntityState&
    {
        return _state;
    }

    void EntityStateComponent::SetState(const GameEntityState& state)
    {
        _state = state;
    }
}
