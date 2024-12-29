#include "circle.h"

namespace sunlight::collision
{
    Circle::Circle(Eigen::Vector2f center, float radius)
        : _center(std::move(center))
        , _radius(radius)
    {
    }

    auto Circle::GetCenter() const -> Eigen::Vector2f
    {
        return _center;
    }

    auto Circle::GetRadius() const -> float
    {
        return _radius;
    }

    void Circle::SetCenter(const Eigen::Vector2f& center)
    {
        _center = center;
    }

    void Circle::SetRadius(float radius)
    {
        _radius = radius;
    }
}
