#pragma once
#include "sl/generator/game/time/game_time.h"

namespace sunlight
{
    struct PathPointMovement
    {
        std::vector<std::pair<game_time_point_type, Eigen::Vector2f>> paths;
        int64_t pathIndex = 0;

        bool IsEnd(game_time_point_type timePoint) const;

        auto CalculatePointOnPath(game_time_point_type timePoint, int64_t& newPathIndex) const -> Eigen::Vector2f;

    private:
        static bool CompareLess(const std::pair<game_time_point_type, Eigen::Vector2f>& lhs, const std::pair<game_time_point_type, Eigen::Vector2f>& rhs);
    };
}
