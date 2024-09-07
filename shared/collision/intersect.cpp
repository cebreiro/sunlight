#include "intersect.h"

#include <array>
#include <numbers>
#include "shared/collision/shape/sector.h"
#include "shared/collision/shape/circle.h"
#include "shared/collision/shape/obb.h"

namespace sunlight::collision
{
    bool Intersect(const Sector& sector, const Circle& circle)
    {
        if (sector.Contains(circle.GetCenter()))
        {
            return true;
        }

        const auto sectorCenter = sector.GetCenter();
        const auto sectorRadius = sector.GetRadius();
        const auto sectorStartAngle = sector.GetStartAngle();
        const auto sectorEndAngle = sector.GetEndAngle();

        const auto circleCenter = circle.GetCenter();
        const auto circleRadius = circle.GetRadius();

        const std::array<Eigen::Vector2d, 2> arcPoints = {
            sectorCenter + sectorRadius * Eigen::Vector2d(std::cos(sectorStartAngle), std::sin(sectorStartAngle)),
            sectorCenter + sectorRadius * Eigen::Vector2d(std::cos(sectorEndAngle), std::sin(sectorEndAngle))
        };

        for (const Eigen::Vector2d& point : arcPoints)
        {
            Eigen::Vector2d toCenter = circleCenter - point;

            if (toCenter.norm() <= circleRadius)
            {
                return true;
            }
        }

        const Eigen::Vector2d toCircle = circleCenter - sectorCenter;

        double angleToCircle = std::atan2(toCircle.y(), toCircle.x());
        if (angleToCircle < 0)
        {
            angleToCircle += 2 * std::numbers::pi;
        }

        bool contains = false;
        if (sectorStartAngle < sectorEndAngle)
        {
            contains = angleToCircle >= sectorStartAngle && angleToCircle <= sectorEndAngle;
        }
        else
        {
            contains = (angleToCircle >= sectorStartAngle || angleToCircle <= sectorEndAngle);
        }

        if (contains)
        {
            const double sum = sectorRadius + circleRadius;

            if (toCircle.squaredNorm() <= (sum * sum))
            {
                return true;
            }
        }

        return false;
    }

    bool Intersect(const OBB& obb, const Circle& circle)
    {
        const Eigen::Vector2d diff = circle.GetCenter() - obb.GetCenter();
        const Eigen::Vector2d localCircleCenter = obb.GetRotation().transpose() * diff;

        const double halfSizeX = obb.GetHalfSize().x();
        const double halfSizeY = obb.GetHalfSize().y();

        Eigen::Vector2d clamped;
        clamped.x() = std::max(-halfSizeX, std::min(localCircleCenter.x(), halfSizeX));
        clamped.y() = std::max(-halfSizeY, std::min(localCircleCenter.y(), halfSizeY));

        return (localCircleCenter - clamped).squaredNorm() <= (circle.GetRadius() * circle.GetRadius());
    }

    bool Intersect(const Circle& lhs, const Circle& rhs)
    {
        const double distance = (lhs.GetCenter() - rhs.GetCenter()).norm();

        return distance <= (lhs.GetRadius() + rhs.GetRadius());
    }

    bool Intersect(const Circle& circle, const Sector& sector)
    {
        return Intersect(sector, circle);
    }

    bool Intersect(const Circle& circle, const OBB& obb)
    {
        return Intersect(obb, circle);
    }
}
