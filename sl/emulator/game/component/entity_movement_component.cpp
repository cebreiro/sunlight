#include "entity_movement_component.h"

namespace sunlight
{
    bool EntityMovementComponent::IsMoving() const
    {
        return std::visit([]<typename T>(const T & item) -> bool
        {
            if constexpr (std::is_same_v<T, ClientMovement>)
            {
                return item.movementTypeBitMask == 0x10;
            }
            else
            {
                static_assert(sizeof(T), "not implemented");
            }

        }, _movement);
    }

    auto EntityMovementComponent::GetStartTimePoint() const -> game_time_point_type
    {
        return _startTimePoint;
    }

    auto EntityMovementComponent::GetForwardMovement() -> ClientMovement*
    {
        return std::get_if<ClientMovement>(&_movement);
    }

    auto EntityMovementComponent::GetForwardMovement() const -> const ClientMovement*
    {
        return std::get_if<ClientMovement>(&_movement);
    }

    void EntityMovementComponent::SetIsMoving(bool value)
    {
        std::visit([value]<typename T>(T& item)
        {
            if constexpr (std::is_same_v<T, ClientMovement>)
            {
                item.movementTypeBitMask = value ? 0x10 : 0;
            }
            else
            {
                static_assert(sizeof(T), "not implemented");
            }

        }, _movement);
    }

    void EntityMovementComponent::SetStartTimePoint(game_time_point_type startTimePoint)
    {
        _startTimePoint = startTimePoint;
    }

    void EntityMovementComponent::SetClientMovement(const ClientMovement& movement)
    {
        _movement = movement;
    }
}
