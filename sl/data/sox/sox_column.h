#pragma once
#include "sl/data/sox/sox_value_type.h"

namespace sunlight
{
    struct SoxColumn
    {
        SoxValueType type = SoxValueType::None;
        std::string name;
    };
}
