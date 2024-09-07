#pragma once
#include <cstdint>
#include "shared/type/value_type.h"

namespace sunlight::session
{
    using id_type = ValueType<uint64_t, class SessionIdTag>;
}
