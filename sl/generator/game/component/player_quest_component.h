#pragma once
#include "sl/generator/game/component/game_component.h"
#include "sl/generator/game/contents/quest/quest.h"

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

    private:
        std::unordered_map<int32_t, Quest> _quests;
    };
}
