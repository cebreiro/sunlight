#pragma once
#include <Eigen/Dense>

namespace sunlight::collision
{
    class Sector
    {
    public:
        Sector() = default;
        Sector(Eigen::Vector2f center, float radius, float startAngle, float endAngle);

        bool Contains(const Eigen::Vector2f& point) const;

        auto GetCenter() const -> Eigen::Vector2f;
        auto GetRadius() const -> float;
        auto GetStartAngle() const -> float;
        auto GetEndAngle() const -> float;

        void SetCenter(const Eigen::Vector2f& center);
        void SetRadius(float radius);
        void SetStartAngle(float angle);
        void SetEndAngle(float angle);

    private:
        Eigen::Vector2f _center = {};
        float _radius = 0.0;
        float _startAngle = 0.0;
        float _endAngle = 0.0;
    };
}
