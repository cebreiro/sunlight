#pragma once
#include "sl/generator/game/component/game_component.h"
#include "sl/generator/game/time/game_time.h"

namespace sunlight
{
    class ItemOwnershipComponent final : public GameComponent
    {
    public:
        bool IsOwner(int64_t cid) const;

        void Add(int64_t cid);

        void Clear();

        auto GetRemainDurationMilli(game_time_point_type now) const -> int32_t;
        void SetEndTimePoint(game_time_point_type timePoint);

    private:
        std::vector<int64_t> _owners;
        game_time_point_type _endTimePoint;
    };
}
