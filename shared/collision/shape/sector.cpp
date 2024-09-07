#include "sector.h"

#include <numbers>

namespace sunlight::collision
{
    Sector::Sector(Eigen::Vector2d center, double radius, double startAngle, double endAngle)
        : _center(std::move(center))
        , _radius(radius)
        , _startAngle(startAngle)
        , _endAngle(endAngle)
    {
    }

    bool Sector::Contains(const Eigen::Vector2d& point) const
    {
        const Eigen::Vector2d relativePoint = point - _center;

        if (relativePoint.squaredNorm() > (_radius * _radius))
        {
            return false;
        }

        double angle = std::atan2(relativePoint.y(), relativePoint.x());
        if (angle < 0)
        {
            angle += 2 * std::numbers::pi;
        }

        if (_startAngle < _endAngle)
        {
            return angle >= _startAngle && angle <= _endAngle;
        }

        return angle >= _startAngle || angle <= _endAngle;
    }

    auto Sector::GetCenter() const -> Eigen::Vector2d
    {
        return _center;
    }

    auto Sector::GetRadius() const -> double
    {
        return _radius;
    }

    auto Sector::GetStartAngle() const -> double
    {
        return _startAngle;
    }

    auto Sector::GetEndAngle() const -> double
    {
        return _endAngle;
    }

    void Sector::SetCenter(const Eigen::Vector2d& center)
    {
        _center = center;
    }

    void Sector::SetRadius(double radius)
    {
        _radius = radius;
    }

    void Sector::SetStartAngle(double angle)
    {
        _startAngle = angle;
    }

    void Sector::SetEndAngle(double angle)
    {
        _endAngle = angle;
    }
}
