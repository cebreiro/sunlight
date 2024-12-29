#include "item_ownership_component.h"

namespace sunlight
{
    bool ItemOwnershipComponent::IsOwner(int64_t cid) const
    {
        if (_owners.empty())
        {
            return false;
        }

        return std::ranges::contains(_owners, cid);
    }

    void ItemOwnershipComponent::Add(int64_t cid)
    {
        _owners.push_back(cid);
    }

    void ItemOwnershipComponent::Clear()
    {
        _owners.clear();
    }

    auto ItemOwnershipComponent::GetRemainDurationMilli(game_time_point_type now) const -> int32_t
    {
        if (_owners.empty())
        {
            return 0;
        }

        if (now >= _endTimePoint)
        {
            return 0;
        }

        const auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(_endTimePoint - now);

        return static_cast<int32_t>(duration.count());
    }

    void ItemOwnershipComponent::SetEndTimePoint(game_time_point_type timePoint)
    {
        _endTimePoint = timePoint;
    }
}
