#include "lua_player.h"

#include <sol/state.hpp>

#include "sl/emulator/game/contants/npc/npc_talk_box.h"
#include "sl/emulator/game/contants/quest/quest_change.h"
#include "sl/emulator/game/component/player_item_component.h"
#include "sl/emulator/game/component/player_job_component.h"
#include "sl/emulator/game/component/player_npc_script_component.h"
#include "sl/emulator/game/component/player_quest_component.h"
#include "sl/emulator/game/component/player_stat_component.h"
#include "sl/emulator/game/entity/game_player.h"
#include "sl/emulator/game/script/class/lua_npc.h"
#include "sl/emulator/game/system/item_archive_system.h"
#include "sl/emulator/game/system/player_quest_system.h"
#include "sl/emulator/game/system/player_state_system.h"

namespace sunlight
{
    LuaPlayer::LuaPlayer(PlayerStateSystem& system, GamePlayer& player)
        : _system(system)
        , _player(player)
    {
    }

    bool LuaPlayer::IsMale() const
    {
        return _player.GetStatComponent().GetGender() == 0;
    }

    bool LuaPlayer::HasInventoryItem(int32_t itemId, int32_t quantity) const
    {
        return _player.GetItemComponent().HasInventoryItem(itemId, quantity);
    }

    bool LuaPlayer::AddItem(int32_t itemId, int32_t quantity)
    {
        return _system.Get<ItemArchiveSystem>().AddItem(_player, itemId, quantity);
    }

    bool LuaPlayer::RemoveInventoryItem(int32_t itemId, int32_t quantity)
    {
        return _system.Get<ItemArchiveSystem>().RemoveInventoryItem(_player, itemId, quantity);
    }

    void LuaPlayer::Talk(LuaNPC& npc, const NPCTalkBox& talkBox)
    {
        _system.CreateNPCTalkBox(_player, npc.GetImpl(), talkBox);
    }

    void LuaPlayer::DisposeTalk()
    {
        _system.DisposeNPCTalk(_player);
    }

    void LuaPlayer::StartQuest(Quest quest)
    {
        _system.Get<PlayerQuestSystem>().StartQuest(_player, quest);
    }

    void LuaPlayer::ChangeQuest(int32_t questId, const QuestChange& change)
    {
        _system.Get<PlayerQuestSystem>().ChangeQuest(_player, questId, change);
    }

    auto LuaPlayer::FindQuest(int32_t quest) const -> const Quest*
    {
        return _player.GetQuestComponent().FindQuest(quest);
    }

    auto LuaPlayer::GetNoviceJobLevel() const -> int32_t
    {
        const Job* novice = _player.GetJobComponent().Find(JobType::Novice);

        return novice ? novice->GetLevel() : 0;
    }

    auto LuaPlayer::GetSelection() const -> int32_t
    {
        return _player.GetNPCScriptComponent().GetSelection();
    }

    void LuaPlayer::Bind(sol::state& luaState)
    {
        luaState.new_usertype<LuaPlayer>("Player",
            sol::no_constructor,
            "isMale", &LuaPlayer::IsMale,
            "hasInventoryItem", &LuaPlayer::HasInventoryItem,
            "addItem", &LuaPlayer::AddItem,
            "removeInventoryItem", &LuaPlayer::RemoveInventoryItem,
            "talk", &LuaPlayer::Talk,
            "disposeTalk", &LuaPlayer::DisposeTalk,
            "startQuest", &LuaPlayer::StartQuest,
            "changeQuest", &LuaPlayer::ChangeQuest,
            "findQuest", &LuaPlayer::FindQuest,
            "getNoviceJobLevel", &LuaPlayer::GetNoviceJobLevel,
            "getSelection", &LuaPlayer::GetSelection
        );
    }

    auto LuaPlayer::GetCId() const -> int64_t
    {
        return _player.GetCId();
    }
}
