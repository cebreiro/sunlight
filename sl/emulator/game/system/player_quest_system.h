#pragma once
#include "sl/emulator/game/system/game_system.h"
#include "sl/emulator/game/zone/stage_enter_type.h"

namespace sunlight
{
    struct QuestItemGain;

    class Quest;
    class QuestChange;
    class GamePlayer;
}

namespace sunlight
{
    class PlayerQuestSystem final : public GameSystem
    {
    public:
        PlayerQuestSystem();

        void InitializeSubSystem(Stage& stage) override;
        bool Subscribe(Stage& stage) override;
        auto GetName() const -> std::string_view override;
        auto GetClassId() const -> game_system_id_type override;

    public:
        void OnStageEnter(GamePlayer& player, StageEnterType enterType);

        void OnMonsterKill(GamePlayer& player, int32_t monsterId);

    public:
        bool StartQuest(GamePlayer& player, Quest& newQuest);
        bool ChangeQuest(GamePlayer& player, int32_t questId, const QuestChange& change);

    private:
        std::mt19937 _mt19937;
        std::vector<PtrNotNull<QuestItemGain>> _questItemGainsBuffer;
    };
}
