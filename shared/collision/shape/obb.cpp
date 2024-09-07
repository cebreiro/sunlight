#include "obb.h"

namespace sunlight::collision
{
    OBB::OBB(Eigen::Vector2d center, Eigen::Vector2d halfSize, Eigen::Matrix2d rotation)
        : _center(std::move(center))
        , _halfSize(std::move(halfSize))
        , _rotation(std::move(rotation))
    {
    }

    auto OBB::GetCenter() const -> Eigen::Vector2d
    {
        return _center;
    }

    auto OBB::GetHalfSize() const -> Eigen::Vector2d
    {
        return _halfSize;
    }

    auto OBB::GetRotation() const -> Eigen::Matrix2d
    {
        return _rotation;
    }

    OBB3d::OBB3d(const Eigen::Vector3d& center, const Eigen::Vector3d& halfSize, const Eigen::Matrix3d& rotation)
        : _center(center)
        , _halfSize(halfSize)
        , _rotation(rotation)
    {
    }

    auto OBB3d::Project() const -> OBB
    {
        Eigen::Matrix2d rotation2D;
        rotation2D << _rotation(0, 0), _rotation(0, 1),
            _rotation(1, 0), _rotation(1, 1);

        return OBB(
            Eigen::Vector2d(_center.x(), _center.y()),
            Eigen::Vector2d(_halfSize.x(), _halfSize.y()),
            rotation2D);
    }

    auto OBB3d::GetCenter() const -> const Eigen::Vector3d&
    {
        return _center;
    }

    auto OBB3d::GetHalfSize() const -> const Eigen::Vector3d&
    {
        return _halfSize;
    }

    auto OBB3d::GetRotation() const -> const Eigen::Matrix3d&
    {
        return _rotation;
    }
}
