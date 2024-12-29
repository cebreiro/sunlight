#include "event_object_stage_entrance_portal_component.h"

namespace sunlight
{
    EventObjectStageEntrancePortalComponent::EventObjectStageEntrancePortalComponent(int32_t linkId)
        : _linkId(linkId)
    {
    }

    auto EventObjectStageEntrancePortalComponent::GetLinkId() const -> int32_t
    {
        return _linkId;
    }
}
