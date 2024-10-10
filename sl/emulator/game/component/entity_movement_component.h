#pragma once
#include "sl/emulator/game/component/game_component.h"
#include "sl/emulator/game/contents/movement/client_movement.h"
#include "sl/emulator/game/time/game_time.h"

namespace sunlight
{
    class EntityMovementComponent final : public GameComponent
    {
    public:
        bool IsMoving() const;

        auto GetStartTimePoint() const -> game_time_point_type;
        auto GetForwardMovement() -> ClientMovement*;
        auto GetForwardMovement() const -> const ClientMovement*;

        void SetIsMoving(bool value);
        void SetStartTimePoint(game_time_point_type startTimePoint);
        void SetForwardMovement(const ClientMovement& movement);

    private:
        game_time_point_type _startTimePoint;
        std::variant<ClientMovement> _movement;
    };
}
