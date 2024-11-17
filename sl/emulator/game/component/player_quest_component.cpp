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

    void PlayerQuestComponent::SetQuestItemGainIndex(int32_t questId)
    {
        Quest* quest = FindQuest(questId);
        if (!quest)
        {
            return;
        }

        assert(quest->HasItemGain());

        if (const auto [begin, end] = _itemGainIndex.equal_range(quest->GetItemGain().monsterId);
            begin != end)
        {
            const auto iter = std::find_if(begin, end, [quest](const auto& pair)
                {
                    return pair.second == quest;
                });
            if (iter != end)
            {
                return;
            }
        }

        _itemGainIndex.emplace(quest->GetItemGain().monsterId, quest);
    }

    void PlayerQuestComponent::ResetQuestItemGainIndex(int32_t questId)
    {
        Quest* quest = FindQuest(questId);
        if (!quest || !quest->HasItemGain())
        {
            return;
        }

        const auto [begin, end] = _itemGainIndex.equal_range(quest->GetItemGain().monsterId);
        if (begin == end)
        {
            return;
        }

        const auto iter = std::find_if(begin, end, [quest](const auto& pair)
            {
                return pair.second == quest;
            });
        if (iter == end)
        {
            return;
        }

        _itemGainIndex.erase(iter);
    }

    bool PlayerQuestComponent::GetItemGain(int32_t monsterId, std::vector<PtrNotNull<QuestItemGain>>& result)
    {
        const auto [begin, end] = _itemGainIndex.equal_range(monsterId);
        if (begin == end)
        {
            return false;
        }

        for (auto iter = begin; iter != end; ++iter)
        {
            assert(iter->second);
            assert(iter->second->HasItemGain());

            result.emplace_back(&iter->second->GetItemGain());
        }

        return true;
    }
}
