#include "sector.h"

#include <numbers>

namespace sunlight::collision
{
    Sector::Sector(Eigen::Vector2f center, float radius, float startAngle, float endAngle)
        : _center(std::move(center))
        , _radius(radius)
        , _startAngle(startAngle)
        , _endAngle(endAngle)
    {
    }

    bool Sector::Contains(const Eigen::Vector2f& point) const
    {
        const Eigen::Vector2f relativePoint = point - _center;

        if (relativePoint.squaredNorm() > (_radius * _radius))
        {
            return false;
        }

        float angle = std::atan2(relativePoint.y(), relativePoint.x());
        if (angle < 0)
        {
            angle += 2 * static_cast<float>(std::numbers::pi);
        }

        if (_startAngle < _endAngle)
        {
            return angle >= _startAngle && angle <= _endAngle;
        }

        return angle >= _startAngle || angle <= _endAngle;
    }

    auto Sector::GetCenter() const -> Eigen::Vector2f
    {
        return _center;
    }

    auto Sector::GetRadius() const -> float
    {
        return _radius;
    }

    auto Sector::GetStartAngle() const -> float
    {
        return _startAngle;
    }

    auto Sector::GetEndAngle() const -> float
    {
        return _endAngle;
    }

    void Sector::SetCenter(const Eigen::Vector2f& center)
    {
        _center = center;
    }

    void Sector::SetRadius(float radius)
    {
        _radius = radius;
    }

    void Sector::SetStartAngle(float angle)
    {
        _startAngle = angle;
    }

    void Sector::SetEndAngle(float angle)
    {
        _endAngle = angle;
    }
}
