#pragma once
#include "sl/data/abf/ability_value.h"

namespace sunlight
{
    struct AbilityProperty
    {
        int32_t unk1 = 0;

        std::vector<AbilityValue> values;
    };
}
