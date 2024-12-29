#include "server_command_quest.h"

#include "sl/generator/game/contents/quest/quest.h"
#include "sl/generator/game/contents/quest/quest_change.h"
#include "sl/generator/game/system/player_quest_system.h"
#include "sl/generator/game/system/server_command_system.h"

namespace sunlight
{
    ServerCommandQuestAdd::ServerCommandQuestAdd(ServerCommandSystem& system)
        : _system(system)
    {
    }

    auto ServerCommandQuestAdd::GetName() const -> std::string_view
    {
        return "quest_add";
    }

    auto ServerCommandQuestAdd::GetRequiredGmLevel() const -> int8_t
    {
        return 0;
    }

    bool ServerCommandQuestAdd::Execute(GamePlayer& player, int32_t questId) const
    {
        Quest quest(questId);

        return _system.Get<PlayerQuestSystem>().StartQuest(player, quest);
    }

    ServerCommandQuestStateSet::ServerCommandQuestStateSet(ServerCommandSystem& system)
        : _system(system)
    {
    }

    auto ServerCommandQuestStateSet::GetName() const -> std::string_view
    {
        return "quest_state_set";
    }

    auto ServerCommandQuestStateSet::GetRequiredGmLevel() const -> int8_t
    {
        return 0;
    }

    bool ServerCommandQuestStateSet::Execute(GamePlayer& player, int32_t questId, int32_t state) const
    {
        QuestChange change;
        change.SetState(state);

        return _system.Get<PlayerQuestSystem>().ChangeQuest(player, questId, change);
    }

    ServerCommandQuestFlagSet::ServerCommandQuestFlagSet(ServerCommandSystem& system)
        : _system(system)
    {
    }

    auto ServerCommandQuestFlagSet::GetName() const -> std::string_view
    {
        return "quest_flag_set";
    }

    auto ServerCommandQuestFlagSet::GetRequiredGmLevel() const -> int8_t
    {
        return 0;
    }

    bool ServerCommandQuestFlagSet::Execute(GamePlayer& player, int32_t questId, int32_t index, int32_t value) const
    {
        QuestChange change;
        change.SetFlag(index, value);

        return _system.Get<PlayerQuestSystem>().ChangeQuest(player, questId, change);
    }
}
