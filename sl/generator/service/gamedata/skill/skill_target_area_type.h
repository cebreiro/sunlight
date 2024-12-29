#pragma once

namespace sunlight
{
    ENUM_CLASS(SkillTargetingAreaType, int32_t,
        (None, 0)
        (OneUnit, 1)
        (Sphere, 2)
        (OBB, 4)
    );
}
