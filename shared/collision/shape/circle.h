#pragma once
#include <Eigen/Dense>

namespace sunlight::collision
{
    class Circle
    {
    public:
        Circle() = default;
        Circle(Eigen::Vector2d center, double radius);

        auto GetCenter() const -> Eigen::Vector2d;
        auto GetRadius() const -> double;

        void SetCenter(const Eigen::Vector2d& center);
        void SetRadius(double radius);

    private:
        Eigen::Vector2d _center = {};
        double _radius = 0.0;
    };
}
