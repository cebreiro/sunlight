#include "zone_message_type.h"

namespace sunlight
{
    auto ToString(ZoneMessageType type) -> std::string
    {
        switch (type)
        {
#define SUNLIGHT_ZONE_MESSAGE_ENUM(name, value) case ZoneMessageType::name: return #name;

        SUNLIGHT_ZONE_MESSAGE_ENUM_VALUES

#undef SUNLIGHT_ZONE_MESSAGE_ENUM
        default:;
        }

        return fmt::format("unknown_{}", static_cast<int32_t>(type));
    }
}
