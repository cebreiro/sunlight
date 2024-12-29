#include "event_bubbling_system.h"

namespace sunlight
{
    void EventBubblingSystem::InitializeSubSystem(Stage& stage)
    {
        (void)stage;
    }

    bool EventBubblingSystem::Subscribe(Stage& stage)
    {
        (void)stage;

        return true;
    }

    auto EventBubblingSystem::GetName() const -> std::string_view
    {
        return "event_bubbling";
    }

    auto EventBubblingSystem::GetClassId() const -> game_system_id_type
    {
        return GameSystem::GetClassId<EventBubblingSystem>();
    }

    void EventBubblingSystem::Publish(const std::any& any)
    {
        const auto [begin, end] = _subscribers.equal_range(any.type());
        if (begin == end)
        {
            return;
        }

        for (auto iter = begin; iter != end; ++iter)
        {
            iter->second(any);
        }
    }
}
