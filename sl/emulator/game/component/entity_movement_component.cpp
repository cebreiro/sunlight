#include "entity_movement_component.h"

namespace sunlight
{
    bool EntityMovementComponent::IsMoving() const
    {
        return std::visit([]<typename T>(const T & item) -> bool
        {
            if constexpr (std::is_same_v<T, ForwardMovement>)
            {
                return item.unk3 == 0x10;
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

    auto EntityMovementComponent::GetForwardMovement() -> ForwardMovement*
    {
        return std::get_if<ForwardMovement>(&_movement);
    }

    auto EntityMovementComponent::GetForwardMovement() const -> const ForwardMovement*
    {
        return std::get_if<ForwardMovement>(&_movement);
    }

    void EntityMovementComponent::SetIsMoving(bool value)
    {
        std::visit([value]<typename T>(T& item)
        {
            if constexpr (std::is_same_v<T, ForwardMovement>)
            {
                item.unk3 = value ? 0x10 : 0;
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

    void EntityMovementComponent::SetForwardMovement(const ForwardMovement& movement)
    {
        _movement = movement;
    }
}
