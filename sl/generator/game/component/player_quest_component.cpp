#include "player_quest_component.h"

namespace sunlight
{
    bool PlayerQuestComponent::HasQuest(int32_t questId) const
    {
        return _quests.contains(questId);
    }

    void PlayerQuestComponent::AddQuest(Quest quest)
    {
        assert(!HasQuest(quest.GetId()));

        const int32_t id = quest.GetId();

        _quests.try_emplace(id, std::move(quest));
    }

    auto PlayerQuestComponent::FindQuest(int32_t questId) -> Quest*
    {
        const auto iter = _quests.find(questId);

        return iter != _quests.end() ? &iter->second : nullptr;
    }

    auto PlayerQuestComponent::FindQuest(int32_t questId) const -> const Quest*
    {
        const auto iter = _quests.find(questId);

        return iter != _quests.end() ? &iter->second : nullptr;
    }

    auto PlayerQuestComponent::GetQuests() -> std::unordered_map<int32_t, Quest>&
    {
        return _quests;
    }

    auto PlayerQuestComponent::GetQuests() const -> const std::unordered_map<int32_t, Quest>&
    {
        return _quests;
    }
}
