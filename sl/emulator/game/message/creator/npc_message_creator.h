#pragma once

namespace sunlight
{
    class GamePlayer;
    class GameNPC;
}

namespace sunlight
{
    // client 0x49AD70
    class NPCMessageCreator
    {
    public:
        NPCMessageCreator() = delete;

        static auto CreateChatting(const GameNPC& npc, const std::string& str) -> Buffer;

        static auto CreateTalkBoxCreate(const GameNPC& npc, const GamePlayer& player, int32_t width, int32_t height) -> Buffer;
        static auto CreateTalkBoxClose(const GameNPC& npc) -> Buffer;
        static auto CreateTalkBoxClear(const GameNPC& npc) -> Buffer;

        static auto CreateTalkBoxAddMenu(const GameNPC& npc, int32_t base, int32_t mouseOver, int32_t menuIndex) -> Buffer;
        static auto CreateTalkBoxAddString(const GameNPC& npc, int32_t tableIndex) -> Buffer;
        static auto CreateTalkBoxAddRuntimeIntString(const GameNPC& npc, int32_t tableIndex, int32_t runtimeValue) -> Buffer;
        static auto CreateTalkBoxAddItemName(const GameNPC& npc, int32_t tableIndex, int32_t itemId) -> Buffer;
    };
}
