#pragma once

namespace sunlight::collision
{
    class Sector;
    class Circle;
    class OBB;
}

namespace sunlight::collision
{
    bool Intersect(const Sector& sector, const Circle& circle);
    bool Intersect(const OBB& obb, const Circle& circle);

    bool Intersect(const Circle& lhs, const Circle& rhs);
    bool Intersect(const Circle& circle, const Sector& sector);
    bool Intersect(const Circle& circle, const OBB& obb);
}
