#include "event_object_stage_exit_portal_component.h"

namespace sunlight
{
    EventObjectStageExitPortalComponent::EventObjectStageExitPortalComponent(int32_t linkId)
        : _linkId(linkId)
    {
    }

    auto EventObjectStageExitPortalComponent::GetLinkId() const -> int32_t
    {
        return _linkId;
    }
}
