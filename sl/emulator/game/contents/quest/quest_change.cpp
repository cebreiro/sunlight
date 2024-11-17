#include "quest_change.h"

#include "sl/emulator/game/time/game_time_service.h"

namespace sunlight
{
    QuestChange::QuestChange()
    {
    }

    bool QuestChange::IsResetQuestItemGain() const
    {
        return _questItemGainReset;
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

    void QuestChange::ConfigureItemGain(int32_t monsterId, int32_t itemId, int32_t probability, int32_t maxItemQuantity, int32_t minKillCount)
    {
        _questItemGain.emplace(monsterId, itemId, probability, maxItemQuantity, minKillCount);
    }

    void QuestChange::ResetItemGain()
    {
        _questItemGainReset = true;
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

    auto QuestChange::GetQuestItemGain() const -> const std::optional<QuestItemGain>&
    {
        return _questItemGain;
    }
}
