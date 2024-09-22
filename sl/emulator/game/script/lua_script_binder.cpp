#include "lua_script_binder.h"

#include <sol/sol.hpp>

#include "sl/emulator/game/contants/npc/npc_talk_box.h"
#include "sl/emulator/game/script/class/lua_npc.h"
#include "sl/emulator/game/script/class/lua_player.h"

namespace sunlight
{
    void LuaScriptBinder::Bind(sol::state& luaState)
    {
        luaState.new_usertype<LuaPlayer>("Player",
            sol::no_constructor,
            "talk", &LuaPlayer::Talk,
            "disposeTalk", &LuaPlayer::DisposeTalk,
            "getSelection", &LuaPlayer::GetSelection
        );

        luaState.new_usertype<LuaNPC>("NPC",
            sol::no_constructor
        );

        luaState.new_usertype<NPCTalkBox>("NPCTalkBox",
            sol::constructors<NPCTalkBox(int32_t, int32_t)>(),
            "setContent", &NPCTalkBox::SetContent,
            "setContentWithInt", &NPCTalkBox::SetContentWithInt,
            "setContentWithItem", &NPCTalkBox::SetContentWithIntItem,
            "addString", &NPCTalkBox::AddString,
            "addMenu", &NPCTalkBox::AddMenu
        );
    }
}
