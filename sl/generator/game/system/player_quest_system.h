#pragma once
#include "sl/generator/game/system/game_system.h"
#include "sl/generator/game/zone/stage_enter_type.h"

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
        explicit PlayerQuestSystem(const ServiceLocator& serviceLocator);

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
        const ServiceLocator& _serviceLocator;

        std::mt19937 _mt19937;
    };
}
