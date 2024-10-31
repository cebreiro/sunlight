#pragma once
#include "sl/emulator/game/component/game_component.h"

namespace sunlight
{
    class EventObjectZonePortalComponent final : public GameComponent
    {
    public:
        EventObjectZonePortalComponent(int32_t destZoneId, int32_t linkId);

        auto GetDestZoneId() const -> int32_t;
        auto GetLinkId() const -> int32_t;

    private:
        int32_t _destZoneId = 0;
        int32_t _linkId = 0;
    };
}
