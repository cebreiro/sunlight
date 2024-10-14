#include "obb.h"

namespace sunlight::collision
{
    OBB::OBB(Eigen::Vector2f center, Eigen::Vector2f halfSize, Eigen::Matrix2f rotation)
        : _center(std::move(center))
        , _halfSize(std::move(halfSize))
        , _rotation(std::move(rotation))
    {
    }

    auto OBB::GetCenter() const -> Eigen::Vector2f
    {
        return _center;
    }

    auto OBB::GetHalfSize() const -> Eigen::Vector2f
    {
        return _halfSize;
    }

    auto OBB::GetRotation() const -> Eigen::Matrix2f
    {
        return _rotation;
    }

    OBB3f::OBB3f(const Eigen::Vector3f& center, const Eigen::Vector3f& halfSize, const Eigen::Matrix3f& rotation)
        : _center(center)
        , _halfSize(halfSize)
        , _rotation(rotation)
    {
    }

    auto OBB3f::Project() const -> OBB
    {
        Eigen::Matrix2f rotation2D;
        rotation2D << _rotation(0, 0), _rotation(0, 1),
            _rotation(1, 0), _rotation(1, 1);

        return OBB(
            Eigen::Vector2f(_center.x(), _center.y()),
            Eigen::Vector2f(_halfSize.x(), _halfSize.y()),
            rotation2D);
    }

    auto OBB3f::GetCenter() const -> const Eigen::Vector3f&
    {
        return _center;
    }

    auto OBB3f::GetHalfSize() const -> const Eigen::Vector3f&
    {
        return _halfSize;
    }

    auto OBB3f::GetRotation() const -> const Eigen::Matrix3f&
    {
        return _rotation;
    }
}
