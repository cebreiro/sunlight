#include "lua_player.h"

#include <sol/state.hpp>

#include "sl/emulator/game/component/player_npc_script_component.h"
#include "sl/emulator/game/entity/game_player.h"
#include "sl/emulator/game/script/class/lua_npc.h"
#include "sl/emulator/game/system/player_state_system.h"

namespace sunlight
{
    LuaPlayer::LuaPlayer(PlayerStateSystem& system, GamePlayer& player)
        : _system(system)
        , _player(player)
    {
    }

    void LuaPlayer::Talk(LuaNPC& npc, const NPCTalkBox& talkBox)
    {
        _system.CreateNPCTalkBox(_player, npc.GetImpl(), talkBox);
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
}
