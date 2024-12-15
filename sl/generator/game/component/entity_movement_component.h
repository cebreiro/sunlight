#pragma once
#include "sl/generator/game/component/game_component.h"
#include "sl/generator/game/contents/movement/client_movement.h"
#include "sl/generator/game/contents/movement/path_point_movement.h"
#include "sl/generator/game/time/game_time.h"

namespace sunlight
{
    class EntityMovementComponent final : public GameComponent
    {
    public:
        bool IsMoving() const;

        auto GetStartTimePoint() const -> game_time_point_type;
        auto GetLastSyncTimePoint() const -> game_time_point_type;

        auto GetClientMovement() -> ClientMovement*;
        auto GetClientMovement() const -> const ClientMovement*;

        auto GetPathPointMovement() -> PathPointMovement*;
        auto GetPathPointMovement() const -> const PathPointMovement*;

        void Reset();

        void SetStartTimePoint(game_time_point_type startTimePoint);
        void SetLastSyncTimePoint(game_time_point_type timePoint);
        void SetClientMovement(const ClientMovement& movement);
        void SetPathPointMovement(PathPointMovement movement);

    private:
        game_time_point_type _startTimePoint;
        game_time_point_type _lastSyncTimePoint;
        std::variant<ClientMovement, PathPointMovement> _movement;
    };
}
