#include "game_spatial_mbr.h"

#include "shared/collision/shape/obb.h"

namespace sunlight
{
    GameSpatialMBR::GameSpatialMBR(const Eigen::Vector2f& min, const Eigen::Vector2f& max)
    {
        _box.min() = min;
        _box.max() = max;
    }

    GameSpatialMBR::GameSpatialMBR(const Eigen::Vector2f& pos, double radius)
        : GameSpatialMBR(
            Eigen::Vector2f(pos.x() - radius, pos.y() - radius),
            Eigen::Vector2f(pos.x() + radius, pos.y() + radius))
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

    auto GameSpatialMBR::GetMin() const -> Eigen::Vector2f
    {
        return _box.min();
    }

    auto GameSpatialMBR::GetMax() const -> Eigen::Vector2f
    {
        return _box.max();
    }

    auto GameSpatialMBR::CreateFrom(const collision::OBB& obb) -> GameSpatialMBR
    {
        const auto getVertices = [](const collision::OBB& obb) -> std::array<Eigen::Vector2f, 4>
            {
                std::array<Eigen::Vector2f, 4> vertices;

                const Eigen::Vector2f& center = obb.GetCenter();
                const Eigen::Vector2f& extents = obb.GetHalfSize();
                const Eigen::Matrix2f& rotation = obb.GetRotation();

                const Eigen::Vector2f axis1 = rotation * Eigen::Vector2f(extents.x(), 0);
                const Eigen::Vector2f axis2 = rotation * Eigen::Vector2f(0, extents.y());

                vertices[0] = center + axis1 + axis2;
                vertices[1] = center + axis1 - axis2;
                vertices[2] = center - axis1 + axis2;
                vertices[3] = center - axis1 - axis2;

                return vertices;
            };

        Eigen::Vector2f min;
        Eigen::Vector2f max;

        for (const Eigen::Vector2f& vertex : getVertices(obb))
        {
            min = min.cwiseMin(vertex);
            max = max.cwiseMax(vertex);
        }

        return GameSpatialMBR(min, max);
    }
}
