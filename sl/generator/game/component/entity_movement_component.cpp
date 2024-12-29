#include "entity_movement_component.h"

#include "sl/generator/game/time/game_time_service.h"

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
            else if constexpr (std::is_same_v<T, PathPointMovement>)
            {
                return !item.paths.empty();
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

    auto EntityMovementComponent::GetLastSyncTimePoint() const -> game_time_point_type
    {
        return _lastSyncTimePoint;
    }

    auto EntityMovementComponent::GetClientMovement() -> ClientMovement*
    {
        return std::get_if<ClientMovement>(&_movement);
    }

    auto EntityMovementComponent::GetClientMovement() const -> const ClientMovement*
    {
        return std::get_if<ClientMovement>(&_movement);
    }

    auto EntityMovementComponent::GetPathPointMovement() -> PathPointMovement*
    {
        return std::get_if<PathPointMovement>(&_movement);
    }

    auto EntityMovementComponent::GetPathPointMovement() const -> const PathPointMovement*
    {
        return std::get_if<PathPointMovement>(&_movement);
    }

    void EntityMovementComponent::Reset()
    {
        std::visit([]<typename T>(T& item)
        {
            if constexpr (std::is_same_v<T, ClientMovement>)
            {
                item.movementTypeBitMask = 0;
            }
            else if constexpr (std::is_same_v<T, PathPointMovement>)
            {
                return item.paths.clear();
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

    void EntityMovementComponent::SetLastSyncTimePoint(game_time_point_type timePoint)
    {
        _lastSyncTimePoint = timePoint;
    }

    void EntityMovementComponent::SetClientMovement(const ClientMovement& movement)
    {
        _movement = movement;
    }

    void EntityMovementComponent::SetPathPointMovement(PathPointMovement movement)
    {
        _movement = std::move(movement);
    }
}
