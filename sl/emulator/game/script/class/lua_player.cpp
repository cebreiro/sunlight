#include "lua_player.h"

#include <sol/state.hpp>

#include "sl/emulator/game/component/player_npc_script_component.h"
#include "sl/emulator/game/entity/game_player.h"
#include "sl/emulator/game/message/creator/npc_message_creator.h"
#include "sl/emulator/game/script/class/lua_npc.h"
#include "sl/emulator/game/script/class/lua_npc_talk_box.h"
#include "sl/emulator/game/system/player_state_system.h"

namespace sunlight
{
    LuaPlayer::LuaPlayer(PlayerStateSystem& system, GamePlayer& player)
        : _system(system)
        , _player(player)
    {
    }

    void LuaPlayer::Talk(LuaNPC& npc, const LuaNPCTalkBox& talkBox)
    {
        _player.Defer(NPCMessageCreator::CreateTalkBoxClear(npc._npc));

        for (const talk_box_item_type& item : talkBox.GetTalkBoxItems())
        {
            std::visit([&]<typename T>(const T& item)
                {
                    if constexpr (std::is_same_v<T, NPCTalkBoxContent>)
                    {
                        _player.Defer(NPCMessageCreator::CreateTalkBoxAddString(npc._npc, item.tableIndex));
                    }
                    else if constexpr (std::is_same_v<T, NPCTalkBoxContentWithInt>)
                    {
                        _player.Defer(NPCMessageCreator::CreateTalkBoxAddRuntimeIntString(npc._npc, item.tableIndex, item.value));
                    }
                    else if constexpr (std::is_same_v<T, NPCTalkBoxContentWithItem>)
                    {
                        _player.Defer(NPCMessageCreator::CreateTalkBoxAddItemName(npc._npc, item.tableIndex, item.tableIndex));
                    }
                    else if constexpr (std::is_same_v<T, NPCTalkBoxString>)
                    {
                        _player.Defer(NPCMessageCreator::CreateTalkBoxAddString(npc._npc, item.tableIndex));
                    }
                    else if constexpr (std::is_same_v<T, NPCTalkBoxMenu>)
                    {
                        _player.Defer(NPCMessageCreator::CreateTalkBoxAddMenu(npc._npc, item.tableIndexDefault, item.tableIndexMouseOver, item.index));
                    }
                    else
                    {
                        static_assert(sizeof(T), "not implemented");
                    }

                }, item);
        }

        _player.Defer(NPCMessageCreator::CreateTalkBoxCreate(npc._npc, _player,
            talkBox.GetWidth(), talkBox.GetHeight()));
        _player.FlushDeferred();
    }

    void LuaPlayer::DisposeTalk()
    {
        _system.DisposeNPCTalk(_player);
    }

    auto LuaPlayer::GetSelection() const -> int32_t
    {
        return _player.GetNPCScriptComponent().GetSelection();
    }

    auto LuaPlayer::GetCId() const -> int64_t
    {
        return _player.GetCId();
    }

    void LuaPlayer::Bind(sol::state& luaState)
    {
        luaState.new_usertype<LuaPlayer>("Player",
            sol::no_constructor,
            "talk", &LuaPlayer::Talk,
            "disposeTalk", &LuaPlayer::DisposeTalk,
            "getSelection", &LuaPlayer::GetSelection
        );
    }
}
