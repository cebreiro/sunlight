#include "player_profile_component.h"

namespace sunlight
{
    bool PlayerProfileComponent::IsPrivate() const
    {
        return _private;
    }

    void PlayerProfileComponent::SetPrivate(bool value)
    {
        _private = value;
    }
}
