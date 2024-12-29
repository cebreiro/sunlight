#pragma once
#include "sl/generator/game/entity/game_entity_id_type.h"
#include "sl/generator/game/entity/game_entity_type.h"

namespace sunlight
{
    class GamePlayer;
    class GameItem;
}

namespace sunlight
{
    class ItemTradeMessageCreator
    {
    public:
        ItemTradeMessageCreator() = delete;

        static auto CreateGroupCreate(int32_t groupId) -> Buffer;

        static auto CreateGroupGuestData(int32_t groupId, const GamePlayer& guest) -> Buffer;
        static auto CreateGroupHostData(int32_t groupId, const GamePlayer& host) -> Buffer;
        static auto CreateGroupHostItemData(int32_t groupId, const GamePlayer& host) -> Buffer;

        static auto CreateGroupJoinFail(int32_t groupId) -> Buffer;
        static auto CreateGroupGuestExit(int32_t groupId, const GamePlayer& host) -> Buffer;

        static auto CreateGoldChange(int32_t groupId, int32_t gold) -> Buffer;
        static auto CreateGoldChangeResult(int32_t groupId) -> Buffer;

        static auto CreateLiftItem(int32_t groupId, game_entity_id_type itemId, GameEntityType itemType) -> Buffer;
        static auto CreateLowerItem(int32_t groupId, int32_t x, int32_t y, const GameItem& item) -> Buffer;

        static auto CreateLiftItemResult(int32_t groupId) -> Buffer;
        static auto CreateLowerItemResult(int32_t groupId) -> Buffer;

        static auto CreateTradeConfirm(int32_t groupId) -> Buffer;

        static auto CreateTradeSuccess(int32_t groupId) -> Buffer;
        static auto CreateTradeFail(int32_t groupId) -> Buffer;
    };
}
