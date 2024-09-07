#pragma once
#include <Eigen/Dense>

namespace sunlight::collision
{
    class Sector
    {
    public:
        Sector() = default;
        Sector(Eigen::Vector2d center, double radius, double startAngle, double endAngle);

        bool Contains(const Eigen::Vector2d& point) const;

        auto GetCenter() const -> Eigen::Vector2d;
        auto GetRadius() const -> double;
        auto GetStartAngle() const -> double;
        auto GetEndAngle() const -> double;

        void SetCenter(const Eigen::Vector2d& center);
        void SetRadius(double radius);
        void SetStartAngle(double angle);
        void SetEndAngle(double angle);

    private:
        Eigen::Vector2d _center = {};
        double _radius = 0.0;
        double _startAngle = 0.0;
        double _endAngle = 0.0;
    };
}
