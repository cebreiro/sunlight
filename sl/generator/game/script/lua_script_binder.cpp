#include "lua_script_binder.h"

#include <sol/sol.hpp>

#include "sl/generator/game/contents/event_script/event_script.h"
#include "sl/generator/game/contents/npc/npc_talk_box.h"
#include "sl/generator/game/contents/quest/quest.h"
#include "sl/generator/game/contents/quest/quest_change.h"
#include "sl/generator/game/script/class/lua_npc.h"
#include "sl/generator/game/script/class/lua_player.h"
#include "sl/generator/game/script/class/lua_system.h"
#include "sl/generator/server/packet/io/sl_packet_writer.h"

namespace sunlight
{
    void LuaScriptBinder::Bind(sol::state& luaState)
    {
        LuaPlayer::Bind(luaState);
        LuaNPC::Bind(luaState);
        LuaMonster::Bind(luaState);
        LuaSystem::Bind(luaState);

        BindPacket(luaState);

        luaState.new_usertype<NPCTalkBox>("NPCTalkBox",
            sol::constructors<NPCTalkBox(int32_t, int32_t)>(),
            "addStringWithInt", &NPCTalkBox::AddStringWithInt,
            "addStringWithItem", &NPCTalkBox::AddStringWithIntItem,
            "addString", &NPCTalkBox::AddString,
            "addMenu", &NPCTalkBox::AddMenu
        );

        luaState.new_usertype<Quest>("Quest",
            sol::constructors<Quest(int32_t)>(),
            "isExpired", &Quest::IsExpired,
            "hasFlag", &Quest::HasFlag,
            "hasTimeLimit", &Quest::HasTimeLimit,
            "getId", &Quest::GetId,
            "getState", &Quest::GetState,
            "getFlag", &Quest::GetFlag,
            "setState", &Quest::SetState,
            "setFlag", &Quest::SetFlag
        );

        luaState.new_usertype<QuestChange>("QuestChange",
            sol::constructors<QuestChange()>(),
            "setState", &QuestChange::SetState,
            "setFlag", &QuestChange::SetFlag,
            "configureTimeLimit", &QuestChange::ConfigureTimeLimit
        );

        luaState.new_usertype<EventScript>("EventScript",
            sol::default_constructor,
            "addString", &EventScript::AddString,
            "addStringWithInt", &EventScript::AddStringWithInt,
            "addStringWithItem", &EventScript::AddStringWithIntItem
        );
    }

    void LuaScriptBinder::BindPacket(sol::state& luaState)
    {
        luaState.new_usertype<PacketWriter>("PacketWriter",
            sol::default_constructor,
            "writeInt8", sol::overload(static_cast<void(PacketWriter::*)(int8_t)>(&PacketWriter::Write)),
            "writeInt16", sol::overload(static_cast<void(PacketWriter::*)(int16_t)>(&PacketWriter::Write)),
            "writeInt32", sol::overload(static_cast<void(PacketWriter::*)(int32_t)>(&PacketWriter::Write)),
            "writeUInt8", sol::overload(static_cast<void(PacketWriter::*)(uint8_t)>(&PacketWriter::Write)),
            "writeUInt16", sol::overload(static_cast<void(PacketWriter::*)(uint16_t)>(&PacketWriter::Write)),
            "writeUInt32", sol::overload(static_cast<void(PacketWriter::*)(uint32_t)>(&PacketWriter::Write)),
            "writeFloat", sol::overload(static_cast<void(PacketWriter::*)(float)>(&PacketWriter::Write)),
            "writeString", &PacketWriter::WriteString,
            "writeFixedSizeString", &PacketWriter::WriteFixeSizeString,
            "writeZeroBytes", &PacketWriter::WriteZeroBytes
        );

        luaState.new_usertype<SlPacketWriter>("SlPacketWriter",
            sol::default_constructor,
            "writeInt8", &SlPacketWriter::Write<int8_t>,
            "writeInt16", &SlPacketWriter::Write<int16_t>,
            "writeInt32", &SlPacketWriter::Write<int32_t>,
            "writeInt64", &SlPacketWriter::WriteInt64,
            "writeUInt8", &SlPacketWriter::Write<uint8_t>,
            "writeUInt16", &SlPacketWriter::Write<uint16_t>,
            "writeUInt32", &SlPacketWriter::Write<uint32_t>,
            "writeString", &SlPacketWriter::WriteString,
            "writeObject", sol::resolve<void(PacketWriter&)>(&SlPacketWriter::WriteObject)
        );
    }
}
