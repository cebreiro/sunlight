#include "lua_script_binder.h"

#include <sol/sol.hpp>

#include "sl/emulator/game/contants/npc/npc_talk_box.h"
#include "sl/emulator/game/contants/quest/quest.h"
#include "sl/emulator/game/contants/quest/quest_change.h"
#include "sl/emulator/game/script/class/lua_npc.h"
#include "sl/emulator/game/script/class/lua_player.h"
#include "sl/emulator/game/script/class/lua_system.h"

namespace sunlight
{
    void LuaScriptBinder::Bind(sol::state& luaState)
    {
        LuaPlayer::Bind(luaState);
        LuaNPC::Bind(luaState);
        LuaSystem::Bind(luaState);

        luaState.new_usertype<NPCTalkBox>("NPCTalkBox",
            sol::constructors<NPCTalkBox(int32_t, int32_t)>(),
            "setContent", &NPCTalkBox::SetContent,
            "setContentWithInt", &NPCTalkBox::SetContentWithInt,
            "setContentWithItem", &NPCTalkBox::SetContentWithIntItem,
            "addString", &NPCTalkBox::AddString,
            "addMenu", &NPCTalkBox::AddMenu
        );

        luaState.new_usertype<Quest>("Quest",
            sol::constructors<Quest(int32_t)>(),
            "hasFlag", &Quest::HasFlag,
            "hasData", &Quest::HasData,
            "getId", &Quest::GetId,
            "getState", &Quest::GetState,
            "getFlag", &Quest::GetFlag,
            "getData", &Quest::GetData,
            "setFlag", &Quest::SetFlag
        );

        luaState.new_usertype<QuestChange>("QuestChange",
            sol::constructors<QuestChange()>(),
            "setNewState", &QuestChange::SetNewState,
            "setFlag", &QuestChange::SetFlag,
            "setData", &QuestChange::SetData
        );
    }
}
