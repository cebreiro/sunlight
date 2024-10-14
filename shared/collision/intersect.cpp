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

        const std::array<Eigen::Vector2f, 2> arcPoints = {
            sectorCenter + sectorRadius * Eigen::Vector2f(std::cos(sectorStartAngle), std::sin(sectorStartAngle)),
            sectorCenter + sectorRadius * Eigen::Vector2f(std::cos(sectorEndAngle), std::sin(sectorEndAngle))
        };

        for (const Eigen::Vector2f& point : arcPoints)
        {
            Eigen::Vector2f toCenter = circleCenter - point;

            if (toCenter.norm() <= circleRadius)
            {
                return true;
            }
        }

        const Eigen::Vector2f toCircle = circleCenter - sectorCenter;

        float angleToCircle = std::atan2(toCircle.y(), toCircle.x());
        if (angleToCircle < 0)
        {
            angleToCircle += 2 * static_cast<float>(std::numbers::pi);
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
            const float sum = sectorRadius + circleRadius;

            if (toCircle.squaredNorm() <= (sum * sum))
            {
                return true;
            }
        }

        return false;
    }

    bool Intersect(const OBB& obb, const Circle& circle)
    {
        const Eigen::Vector2f diff = circle.GetCenter() - obb.GetCenter();
        const Eigen::Vector2f localCircleCenter = obb.GetRotation().transpose() * diff;

        const float halfSizeX = obb.GetHalfSize().x();
        const float halfSizeY = obb.GetHalfSize().y();

        Eigen::Vector2f clamped;
        clamped.x() = std::max(-halfSizeX, std::min(localCircleCenter.x(), halfSizeX));
        clamped.y() = std::max(-halfSizeY, std::min(localCircleCenter.y(), halfSizeY));

        return (localCircleCenter - clamped).squaredNorm() <= (circle.GetRadius() * circle.GetRadius());
    }

    bool Intersect(const Circle& lhs, const Circle& rhs)
    {
        const float distance = (lhs.GetCenter() - rhs.GetCenter()).norm();

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
