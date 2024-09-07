#include "circle.h"

namespace sunlight::collision
{
    Circle::Circle(Eigen::Vector2d center, double radius)
        : _center(std::move(center))
        , _radius(radius)
    {
    }

    auto Circle::GetCenter() const -> Eigen::Vector2d
    {
        return _center;
    }

    auto Circle::GetRadius() const -> double
    {
        return _radius;
    }

    void Circle::SetCenter(const Eigen::Vector2d& center)
    {
        _center = center;
    }

    void Circle::SetRadius(double radius)
    {
        _radius = radius;
    }
}
