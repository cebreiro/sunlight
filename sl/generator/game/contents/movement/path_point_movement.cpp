#include "path_point_movement.h"

namespace sunlight
{
    bool PathPointMovement::IsEnd(game_time_point_type timePoint) const
    {
        if (paths.empty())
        {
            return true;
        }

        return timePoint >= paths.rbegin()->first;
    }

    auto PathPointMovement::CalculatePointOnPath(game_time_point_type timePoint, int64_t& newPathIndex) const -> Eigen::Vector2f
    {
        assert(!paths.empty());
        assert(std::ranges::is_sorted(paths, &CompareLess));

        const auto iter = std::ranges::lower_bound(paths, std::pair{ timePoint, Eigen::Vector2f{} }, &CompareLess);
        newPathIndex = std::distance(paths.begin(), iter);

        if (iter == paths.end())
        {
            return paths.rbegin()->second;
        }

        const auto& [endTimePoint, endPos] = *iter;
        if (endTimePoint == timePoint)
        {
            return endPos;
        }

        assert(timePoint < endTimePoint);

        const auto prev = std::prev(iter);
        assert(prev != paths.end());

        const auto& [startTimePoint, startPos] = *prev;
        assert(endTimePoint >= startTimePoint);

        const float numerator = static_cast<float>((timePoint - startTimePoint).count());
        const float denominator = static_cast<float>((endTimePoint - startTimePoint).count());

        const float t = std::clamp(numerator / denominator, 0.f, 1.f);

        return ((1.f - t) * startPos) + (t * endPos);
    }

    bool PathPointMovement::CompareLess(const std::pair<game_time_point_type, Eigen::Vector2f>& lhs, const std::pair<game_time_point_type, Eigen::Vector2f>& rhs)
    {
        return lhs.first < rhs.first;
    }
}
