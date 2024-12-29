#include "player_quest_system.h"

#include "sl/generator/game/component/player_item_component.h"
#include "sl/generator/game/component/player_quest_component.h"
#include "sl/generator/game/contents/quest/quest_change.h"
#include "sl/generator/game/entity/game_player.h"
#include "sl/generator/game/message/creator/game_player_message_creator.h"
#include "sl/generator/game/script/lua_script_engine.h"
#include "sl/generator/game/script/class/lua_player.h"
#include "sl/generator/game/system/event_bubbling_system.h"
#include "sl/generator/game/system/item_archive_system.h"
#include "sl/generator/game/system/event_bubbling/player_event_bubbling.h"
#include "sl/generator/game/time/game_time_service.h"
#include "sl/generator/game/zone/stage.h"
#include "sl/generator/game/zone/service/game_repository_service.h"

namespace sunlight
{
    PlayerQuestSystem::PlayerQuestSystem(const ServiceLocator& serviceLocator)
        : _serviceLocator(serviceLocator)
        , _mt19937(std::random_device{}())
    {
    }

    void PlayerQuestSystem::InitializeSubSystem(Stage& stage)
    {
        Add(stage.Get<EventBubblingSystem>());
        Add(stage.Get<ItemArchiveSystem>());
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

    void PlayerQuestSystem::OnStageEnter(GamePlayer& player, StageEnterType enterType)
    {
        if (enterType == StageEnterType::Login)
        {
            const game_time_point_type now = GameTimeService::Now();

            PlayerQuestComponent& questComponent = player.GetQuestComponent();

            for (Quest& quest : questComponent.GetQuests() | std::views::values)
            {
                if (quest.GetState() == 0)
                {
                    if (quest.HasTimeLimit())
                    {
                        const QuestTimeLimit& timeLimit = quest.GetTimeLimit();
                        const game_time_point_type startTimePoint(game_time_point_type::duration(timeLimit.startTimePoint));
                        const game_time_point_type endTimePoint(game_time_point_type::duration(timeLimit.endTimePoint));

                        const int32_t value = now >= endTimePoint
                            ? std::chrono::duration_cast<std::chrono::minutes>(endTimePoint - startTimePoint).count() + 1
                            : std::chrono::duration_cast<std::chrono::minutes>(now - startTimePoint).count();

                        quest.SetFlag(timeLimit.flagIndex, -value);
                    }
                }
            }
        }
    }

    void PlayerQuestSystem::OnMonsterKill(GamePlayer& player, int32_t monsterId)
    {
        PlayerQuestComponent& questComponent = player.GetQuestComponent();

        for (const Quest& quest : questComponent.GetQuests() | std::views::values)
        {
            if (quest.GetState() == 0 && quest.HasFlagValue(monsterId))
            {
                Get<EventBubblingSystem>().Publish(EventBubblingPlayerQuestMonsterKill{
                    .player = &player,
                    .questId = quest.GetId(),
                    .monsterId = monsterId,
                    });
            }
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

        _serviceLocator.Get<GameRepositoryService>().SaveNewQuest(player, newQuest.GetId(), newQuest.GetState(),
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

        _serviceLocator.Get<GameRepositoryService>().SaveQuestChange(player, quest->GetId(), quest->GetState(),
            quest->GetFlagString(), quest->GetData());

        if (player.HasDeferred())
        {
            player.FlushDeferred();
        }

        return true;
    }
}
