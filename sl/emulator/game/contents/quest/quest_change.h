#pragma once
#include <boost/container/flat_map.hpp>

#include "sl/emulator/game/contents/quest/quest_time_limit.h"

namespace sunlight
{
    class QuestChange
    {
    public:
        QuestChange();

        void SetNewState(int32_t newState);
        void SetFlag(int32_t index, int32_t value);
        void ConfigureTimeLimit(int32_t localIndex, int32_t minute);

        auto GetNewState() const -> std::optional<int32_t>;
        auto GetFlags() const -> const boost::container::small_flat_map<int32_t, int32_t, 4>&;

        auto GetQuestTimeLimit() const -> const std::optional<QuestTimeLimit>&;

    private:
        std::optional<int32_t> _newState = std::nullopt;

        boost::container::small_flat_map<int32_t, int32_t, 4> _flags;

        std::optional<QuestTimeLimit> _questTimeLimit = std::nullopt;
    };
}
