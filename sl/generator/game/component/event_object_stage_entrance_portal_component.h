#pragma once
#include "sl/generator/game/component/game_component.h"

namespace sunlight
{
    class EventObjectStageEntrancePortalComponent final : public GameComponent
    {
    public:
        EventObjectStageEntrancePortalComponent(int32_t linkId);

        auto GetLinkId() const -> int32_t;

    private:
        int32_t _linkId = 0;
        Eigen::AlignedBox2f _collisionBox;
    };
}
