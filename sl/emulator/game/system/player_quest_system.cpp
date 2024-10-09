#include "player_quest_system.h"

#include "sl/emulator/game/component/player_quest_component.h"
#include "sl/emulator/game/contents/quest/quest_change.h"
#include "sl/emulator/game/entity/game_player.h"
#include "sl/emulator/game/message/creator/game_player_message_creator.h"
#include "sl/emulator/game/system/game_repository_system.h"
#include "sl/emulator/game/time/game_time_service.h"
#include "sl/emulator/game/zone/stage.h"

namespace sunlight
{
    void PlayerQuestSystem::InitializeSubSystem(Stage& stage)
    {
        Add(stage.Get<GameRepositorySystem>());
    }

    bool PlayerQuestSystem::Subscribe(Stage& stage)
    {
        (void)stage;

        return true;
    }

    auto PlayerQuestSystem::GetName() const -> std::string_view
    {
        return "player_quest_system";
    }

    auto PlayerQuestSystem::GetClassId() const -> game_system_id_type
    {
        return GameSystem::GetClassId<PlayerQuestSystem>();
    }

    void PlayerQuestSystem::OnInitialize(GamePlayer& player)
    {
        const game_time_point_type now = GameTimeService::Now();

        for (Quest& quest : player.GetQuestComponent().GetQuests() | std::views::values)
        {
            if (!quest.HasTimeLimit())
            {
                continue;
            }

            const QuestTimeLimit& timeLimit = quest.GetTimeLimit();
            const game_time_point_type startTimePoint(game_time_point_type::duration(timeLimit.startTimePoint));
            const game_time_point_type endTimePoint(game_time_point_type::duration(timeLimit.endTimePoint));

            const int32_t value = now >= endTimePoint
                ? std::chrono::duration_cast<std::chrono::minutes>(endTimePoint - startTimePoint).count() + 1
                : std::chrono::duration_cast<std::chrono::minutes>(now - startTimePoint).count();

            quest.SetFlag(timeLimit.flagIndex, -value);
        }
    }

    bool PlayerQuestSystem::StartQuest(GamePlayer& player, Quest& newQuest)
    {
        PlayerQuestComponent& questComponent = player.GetQuestComponent();

        const int32_t questId = newQuest.GetId();

        if (questComponent.HasQuest(questId))
        {
            return false;
        }

        Get<GameRepositorySystem>().SaveNewQuest(player, newQuest.GetId(), newQuest.GetState(),
            newQuest.GetFlagString(), newQuest.GetData());

        player.Send(GamePlayerMessageCreator::CreateQuestAdd(player, newQuest));
        questComponent.AddQuest(std::move(newQuest));

        return true;
    }

    bool PlayerQuestSystem::ChangeQuest(GamePlayer& player, int32_t questId, const QuestChange& change)
    {
        PlayerQuestComponent& questComponent = player.GetQuestComponent();

        Quest* quest = questComponent.FindQuest(questId);
        if (!quest)
        {
            return false;
        }

        const int32_t oldState = quest->GetState();

        if (const std::optional<int32_t> newState = change.GetNewState();
            newState.has_value() && *newState != oldState)
        {
            quest->SetState(*newState);

            player.Defer(GamePlayerMessageCreator::CreateQuestStateChange(player,
                questId, oldState, quest->GetState()));
        }

        for (const auto& [index, flag] : change.GetFlags())
        {
            quest->SetFlag(index, flag);

            player.Defer(GamePlayerMessageCreator::CreateQuestFlagSet(player,
                questId, quest->GetState(), index, flag));
        }

        if (const std::optional<QuestTimeLimit>& timeLimit = change.GetQuestTimeLimit(); timeLimit.has_value())
        {
            quest->SetTimeLimit(timeLimit);
        }

        Get<GameRepositorySystem>().SaveQuestChange(player, quest->GetId(), quest->GetState(),
            quest->GetFlagString(), quest->GetData());

        if (player.HasDeferred())
        {
            player.FlushDeferred();
        }

        return true;
    }
}
