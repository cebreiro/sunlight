#include "game_event_object.h"

namespace sunlight
{
    GameEventObject::GameEventObject(game_entity_id_type id, Eigen::Vector2f center, float yaw, const Eigen::AlignedBox2f& area)
        : GameEntity(id, TYPE)
        , _center(center)
        , _yaw(yaw)
        , _area(area)
    {
    }

    auto GameEventObject::GetCenterPosition() const -> Eigen::Vector2f
    {
        return _center;
    }

    auto GameEventObject::GetYaw() const -> float
    {
        return _yaw;
    }

    auto GameEventObject::GetArea() const -> const Eigen::AlignedBox2f&
    {
        return _area;
    }
}
