#pragma once
#include <Eigen/Dense>

namespace sunlight::collision
{
    class Circle
    {
    public:
        Circle() = default;
        Circle(Eigen::Vector2f center, float radius);

        auto GetCenter() const -> Eigen::Vector2f;
        auto GetRadius() const -> float;

        void SetCenter(const Eigen::Vector2f& center);
        void SetRadius(float radius);

    private:
        Eigen::Vector2f _center = {};
        float _radius = 0.f;
    };
}
