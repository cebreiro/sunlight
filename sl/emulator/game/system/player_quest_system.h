#pragma once
#include "sl/emulator/game/system/game_system.h"

namespace sunlight
{
    class Quest;
    class QuestChange;
    class GamePlayer;
}

namespace sunlight
{
    class PlayerQuestSystem final : public GameSystem
    {
    public:
        void InitializeSubSystem(Stage& stage) override;
        bool Subscribe(Stage& stage) override;
        auto GetName() const -> std::string_view override;
        auto GetClassId() const -> game_system_id_type override;

    public:
        void OnInitialize(GamePlayer& player);

    public:
        bool StartQuest(GamePlayer& player, Quest& newQuest);
        bool ChangeQuest(GamePlayer& player, int32_t questId, const QuestChange& change);
    };
}
