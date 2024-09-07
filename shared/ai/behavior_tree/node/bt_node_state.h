#pragma once
#include <cstdint>
#include "shared/type/enum_class.h"

namespace sunlight::bt::node
{
    ENUM_CLASS(State, int32_t,
        (Success)
        (Failure)
        (Running)
    )
}
