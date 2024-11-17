#pragma once
#include <boost/container/flat_map.hpp>

#include "sl/emulator/game/contents/quest/quest_item_gain.h"
#include "sl/emulator/game/contents/quest/quest_time_limit.h"

namespace sunlight
{
    class QuestChange
    {
    public:
        QuestChange();

        bool IsResetQuestItemGain() const;

        // func_1203
        void SetState(int32_t newState);

        // func_1205
        void SetFlag(int32_t index, int32_t value);
        void ConfigureTimeLimit(int32_t localIndex, int32_t minute);
        void ConfigureItemGain(int32_t monsterId, int32_t itemId, int32_t probability, int32_t maxItemQuantity, int32_t minKillCount);
        void ResetItemGain();

        auto GetNewState() const -> std::optional<int32_t>;
        auto GetFlags() const -> const boost::container::small_flat_map<int32_t, int32_t, 4>&;

        auto GetQuestTimeLimit() const -> const std::optional<QuestTimeLimit>&;
        auto GetQuestItemGain() const -> const std::optional<QuestItemGain>&;

    private:
        std::optional<int32_t> _newState = std::nullopt;

        boost::container::small_flat_map<int32_t, int32_t, 4> _flags;

        std::optional<QuestTimeLimit> _questTimeLimit = std::nullopt;
        std::optional<QuestItemGain> _questItemGain = std::nullopt;
        bool _questItemGainReset = false;
    };
}
