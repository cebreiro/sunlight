#pragma once
#include "sl/generator/game/component/game_component.h"

namespace sunlight
{
    class EventObjectStageExitPortalComponent final : public GameComponent
    {
    public:
        explicit EventObjectStageExitPortalComponent(int32_t linkId);

        auto GetLinkId() const -> int32_t;

    private:
        int32_t _linkId = 0;
        Eigen::Vector2f _position = {};
    };
}
