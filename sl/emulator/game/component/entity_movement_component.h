#pragma once
#include "sl/emulator/game/component/game_component.h"
#include "sl/emulator/game/contants/movement/forward_movement.h"
#include "sl/emulator/game/time/game_time.h"

namespace sunlight
{
    class EntityMovementComponent final : public GameComponent
    {
    public:
        bool IsMoving() const;

        auto GetStartTimePoint() const -> game_time_point_type;
        auto GetForwardMovement() -> ForwardMovement*;
        auto GetForwardMovement() const -> const ForwardMovement*;

        void SetIsMoving(bool value);
        void SetStartTimePoint(game_time_point_type startTimePoint);
        void SetForwardMovement(const ForwardMovement& movement);

    private:
        game_time_point_type _startTimePoint;
        std::variant<ForwardMovement> _movement;
    };
}
