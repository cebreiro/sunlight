#include "quest_change.h"

#include "sl/generator/game/time/game_time_service.h"

namespace sunlight
{
    QuestChange::QuestChange()
    {
    }

    void QuestChange::SetState(int32_t newState)
    {
        _newState = newState;
    }

    void QuestChange::SetFlag(int32_t index, int32_t value)
    {
        _flags[index] = value;
    }

    void QuestChange::ConfigureTimeLimit(int32_t localIndex, int32_t minute)
    {
        QuestTimeLimit& questTimeLimit = _questTimeLimit.emplace();

        questTimeLimit.flagIndex = localIndex;
        questTimeLimit.startTimePoint = GameTimeService::Now().time_since_epoch().count();
        questTimeLimit.endTimePoint = questTimeLimit.startTimePoint
            + std::chrono::duration_cast<game_time_point_type::duration>(std::chrono::minutes(minute)).count();
    }

    auto QuestChange::GetNewState() const -> std::optional<int32_t>
    {
        return _newState;
    }

    auto QuestChange::GetFlags() const -> const boost::container::small_flat_map<int32_t, int32_t, 4>&
    {
        return _flags;
    }

    auto QuestChange::GetQuestTimeLimit() const -> const std::optional<QuestTimeLimit>&
    {
        return _questTimeLimit;
    }
}
