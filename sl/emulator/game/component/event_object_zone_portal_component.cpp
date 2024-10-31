#include "event_object_zone_portal_component.h"

namespace sunlight
{
    EventObjectZonePortalComponent::EventObjectZonePortalComponent(int32_t destZoneId, int32_t linkId)
        : _destZoneId(destZoneId)
        , _linkId(linkId)
    {
    }

    auto EventObjectZonePortalComponent::GetDestZoneId() const -> int32_t
    {
        return _destZoneId;
    }

    auto EventObjectZonePortalComponent::GetLinkId() const -> int32_t
    {
        return _linkId;
    }
}
