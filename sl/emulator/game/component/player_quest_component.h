#pragma once
#include "sl/emulator/game/component/game_component.h"
#include "sl/emulator/game/contents/quest/quest.h"

namespace sunlight
{
    class PlayerQuestComponent final : public GameComponent
    {
    public:
        bool HasQuest(int32_t questId) const;

        void AddQuest(Quest quest);

        auto FindQuest(int32_t questId) -> Quest*;
        auto FindQuest(int32_t questId) const -> const Quest*;

        auto GetQuests() -> std::unordered_map<int32_t, Quest>&;
        auto GetQuests() const -> const std::unordered_map<int32_t, Quest>&;

        void SetQuestItemGainIndex(int32_t questId);
        void ResetQuestItemGainIndex(int32_t questId);

        bool GetItemGain(int32_t monsterId, std::vector<PtrNotNull<QuestItemGain>>& result);

    private:
        std::unordered_map<int32_t, Quest> _quests;
        std::unordered_multimap<int32_t, PtrNotNull<Quest>> _itemGainIndex;
    };
}
