#pragma once
#include <string>

namespace sunlight::bt
{
    template <typename T>
    concept bt_event_concept = requires
    {
        requires std::is_copy_assignable_v<T>;
    };

    struct EventBase
    {
        std::function<void()> postOperation;
    };

    template <bt_event_concept... E>
    struct Event : EventBase
    {
        Event() = default;
        explicit Event(const std::function<void()>& postOperation)
            : EventBase(postOperation)
        {
        }
    };
}
