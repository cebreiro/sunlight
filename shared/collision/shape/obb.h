#pragma once
#include <Eigen/Dense>

namespace sunlight::collision
{
    class OBB
    {
    public:
        OBB() = default;
        OBB(Eigen::Vector2d center, Eigen::Vector2d halfSize, Eigen::Matrix2d rotation);

        auto GetCenter() const -> Eigen::Vector2d;
        auto GetHalfSize() const -> Eigen::Vector2d;
        auto GetRotation() const -> Eigen::Matrix2d;

    private:
        Eigen::Vector2d _center = {};
        Eigen::Vector2d _halfSize = {};
        Eigen::Matrix2d _rotation = {};
    };

    class OBB3d
    {
    public:
        OBB3d(const Eigen::Vector3d& center, const Eigen::Vector3d& halfSize, const Eigen::Matrix3d& rotation);

        auto Project() const -> OBB;

        auto GetCenter() const -> const Eigen::Vector3d&;
        auto GetHalfSize() const -> const Eigen::Vector3d&;
        auto GetRotation() const -> const Eigen::Matrix3d&;

    private:
        Eigen::Vector3d _center = {};
        Eigen::Vector3d _halfSize = {};
        Eigen::Matrix3d _rotation = {};
    };
}
