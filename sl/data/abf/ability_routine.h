#pragma once
#include "sl/data/abf/ability_property.h"

namespace sunlight
{
    struct AbilityRoutine
    {
        int32_t weaponClass = {};
        std::array<int32_t, 7> data = {};
        int32_t time = 0;
        int32_t type = 0;

        std::vector<AbilityProperty> properties;
    };
}
