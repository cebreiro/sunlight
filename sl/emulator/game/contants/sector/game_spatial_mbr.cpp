#include "game_spatial_mbr.h"

#include "shared/collision/shape/obb.h"

namespace sunlight
{
    GameSpatialMBR::GameSpatialMBR(const Eigen::Vector2d& min, const Eigen::Vector2d& max)
    {
        _box.min() = min;
        _box.max() = max;
    }

    GameSpatialMBR::GameSpatialMBR(const Eigen::Vector2d& pos, double radius)
        : GameSpatialMBR(
            Eigen::Vector2d(pos.x() - radius, pos.y() - radius),
            Eigen::Vector2d(pos.x() + radius, pos.y() + radius))
    {
    }

    void GameSpatialMBR::Extend(const GameSpatialMBR& other)
    {
        _box.extend(other._box);
    }

    bool GameSpatialMBR::Intersect(const GameSpatialMBR& other) const
    {
        return _box.intersects(other._box);
    }

    auto GameSpatialMBR::GetMin() const -> Eigen::Vector2d
    {
        return _box.min();
    }

    auto GameSpatialMBR::GetMax() const -> Eigen::Vector2d
    {
        return _box.max();
    }

    auto GameSpatialMBR::CreateFrom(const collision::OBB& obb) -> GameSpatialMBR
    {
        const auto getVertices = [](const collision::OBB& obb) -> std::array<Eigen::Vector2d, 4>
            {
                std::array<Eigen::Vector2d, 4> vertices;

                const Eigen::Vector2d& center = obb.GetCenter();
                const Eigen::Vector2d& extents = obb.GetHalfSize();
                const Eigen::Matrix2d& rotation = obb.GetRotation();

                const Eigen::Vector2d axis1 = rotation * Eigen::Vector2d(extents.x(), 0);
                const Eigen::Vector2d axis2 = rotation * Eigen::Vector2d(0, extents.y());

                vertices[0] = center + axis1 + axis2;
                vertices[1] = center + axis1 - axis2;
                vertices[2] = center - axis1 + axis2;
                vertices[3] = center - axis1 - axis2;

                return vertices;
            };

        Eigen::Vector2d min;
        Eigen::Vector2d max;

        for (const Eigen::Vector2d& vertex : getVertices(obb))
        {
            min = min.cwiseMin(vertex);
            max = max.cwiseMax(vertex);
        }

        return GameSpatialMBR(min, max);
    }
}
