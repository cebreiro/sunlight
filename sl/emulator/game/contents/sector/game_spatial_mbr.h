#pragma once
#include <Eigen/Geometry>

namespace sunlight::collision
{
    class OBB;
}

namespace sunlight
{
    class GameSpatialMBR
    {
    public:
        GameSpatialMBR() = default;
        GameSpatialMBR(const Eigen::Vector2f& min, const Eigen::Vector2f& max);
        GameSpatialMBR(const Eigen::Vector2f& pos, double radius);

        void Extend(const GameSpatialMBR& other);

        bool Intersect(const GameSpatialMBR& other) const;

        auto GetMin() const-> Eigen::Vector2f;
        auto GetMax() const-> Eigen::Vector2f;

        static auto CreateFrom(const collision::OBB& obb) -> GameSpatialMBR;

    private:
        Eigen::AlignedBox2f _box = {};
    };
}
