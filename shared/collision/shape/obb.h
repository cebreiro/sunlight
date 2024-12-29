#pragma once
#include <Eigen/Dense>

namespace sunlight::collision
{
    class OBB
    {
    public:
        OBB() = default;
        OBB(Eigen::Vector2f center, Eigen::Vector2f halfSize, Eigen::Matrix2f rotation);

        auto GetCenter() const -> Eigen::Vector2f;
        auto GetHalfSize() const -> Eigen::Vector2f;
        auto GetRotation() const -> Eigen::Matrix2f;

    private:
        Eigen::Vector2f _center = {};
        Eigen::Vector2f _halfSize = {};
        Eigen::Matrix2f _rotation = {};
    };

    class OBB3f
    {
    public:
        OBB3f(const Eigen::Vector3f& center, const Eigen::Vector3f& halfSize, const Eigen::Matrix3f& rotation);

        auto Project() const -> OBB;

        auto GetCenter() const -> const Eigen::Vector3f&;
        auto GetHalfSize() const -> const Eigen::Vector3f&;
        auto GetRotation() const -> const Eigen::Matrix3f&;

    private:
        Eigen::Vector3f _center = {};
        Eigen::Vector3f _halfSize = {};
        Eigen::Matrix3f _rotation = {};
    };
}
