#pragma once
#include "sl/generator/game/contents/npc/npc_item_shop_result.h"
#include "sl/generator/game/entity/game_entity_id_type.h"
#include "sl/generator/game/entity/game_entity_type.h"

namespace sunlight
{
    class GamePlayer;
    class GameNPC;
    class GameItem;
}

namespace sunlight
{
    class NPCMessageCreator
    {
    public:
        NPCMessageCreator() = delete;

        static auto CreateChatting(const GameNPC& npc, const std::string& str) -> Buffer;

        static auto CreateTalkBoxCreate(const GameNPC& npc, const GamePlayer& player, int32_t width, int32_t height) -> Buffer;
        static auto CreateTalkBoxClose(const GameNPC& npc) -> Buffer;
        static auto CreateTalkBoxClose(game_entity_id_type id) -> Buffer;
        static auto CreateTalkBoxClear(const GameNPC& npc) -> Buffer;

        static auto CreateTalkBoxAddMenu(const GameNPC& npc, int32_t base, int32_t mouseOver, int32_t menuIndex) -> Buffer;
        static auto CreateTalkBoxAddString(const GameNPC& npc, int32_t tableIndex) -> Buffer;
        static auto CreateTalkBoxAddRuntimeIntString(const GameNPC& npc, int32_t tableIndex, int32_t runtimeValue) -> Buffer;
        static auto CreateTalkBoxAddItemName(const GameNPC& npc, int32_t tableIndex, int32_t itemId) -> Buffer;

        static auto CreateShopOpen(const GameNPC& npc) -> Buffer;
        static auto CreateItemSynchroStart(const GameNPC& npc) -> Buffer;

        static auto CreateNPCItemAdd(const GameNPC& npc, const GameItem& item) -> Buffer;
        static auto CreateNPCItemRemove(const GameNPC& npc, game_entity_id_type targetId, GameEntityType targetType) -> Buffer;
        static auto CreateNPCItemDecrease(const GameNPC& npc, const GameItem& item, int32_t newQuantity) -> Buffer;
        static auto CreateNPCItemArchiveResult(const GameNPC& npc, NPCItemShopResult result) -> Buffer;
    };
}
