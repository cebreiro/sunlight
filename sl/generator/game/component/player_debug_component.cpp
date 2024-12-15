#include "player_debug_component.h"

namespace sunlight
{
    bool PlayerDebugComponent::IsSet(PlayerDebugType type) const
    {
        return _bitset.test(static_cast<size_t>(type));
    }

    void PlayerDebugComponent::Toggle(PlayerDebugType type)
    {
        _bitset.flip(static_cast<size_t>(type));
    }
}
