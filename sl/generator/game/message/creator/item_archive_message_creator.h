#pragma once
#include <boost/container/static_vector.hpp>

#include "sl/generator/game/entity/game_entity_id_type.h"
#include "sl/generator/game/entity/game_entity_type.h"
#include "sl/generator/game/message/zone_message_type.h"

namespace sunlight
{
    class GamePlayer;
    class GameItem;
}

namespace sunlight
{
    class ItemArchiveMessageCreator
    {
    public:
        ItemArchiveMessageCreator() = delete;

        static auto CreateInit(const GamePlayer& player) -> Buffer;

        static auto CreateNewPickedItemAdd(const GamePlayer& player, const GameItem& item) -> Buffer;
        static auto CreateItemLift(const GamePlayer& player, const GameItem& pickedItem) -> Buffer;
        static auto CreateItemLift(const GamePlayer& player, const GameItem& pickedItem, const GameItem& origin, int32_t decreaseQuantity) -> Buffer;

        static auto CreateInventoryItemAdd(const GamePlayer& player, const GameItem& item) -> Buffer;
        static auto CreateItemAddForRefresh(const GamePlayer& player, int32_t itemId) -> Buffer;
        static auto CreateItemRemoveForRefresh(const GamePlayer& player) -> Buffer;

        static auto CreateItemDecrease(const GamePlayer& player, const GameItem& item, int32_t quantity) -> Buffer;
        static auto CreateItemDecrease(const GamePlayer& player, game_entity_id_type target, GameEntityType targetType, int32_t quantity) -> Buffer;
        static auto CreateItemRemove(const GamePlayer& player, game_entity_id_type target, GameEntityType targetType) -> Buffer;

        static auto CreateGoldAddOrSub(const GamePlayer& player, int32_t value) -> Buffer;
        static auto CreateArchiveResult(const GamePlayer& player, bool result, ZoneMessageType archiveMessage) -> Buffer;

        static auto CreateAccountStorageOpening(const GamePlayer& player) -> Buffer;
        static auto CreateAccountStorageInit(const GamePlayer& player) -> Buffer;
        static auto CreateAccountStorageUnlock(const GamePlayer& player) -> Buffer;
        static auto CreateAccountStorageGoldAddOrSub(const GamePlayer& player, int32_t value) -> Buffer;

    public:
        using item_object_buffer_type = boost::container::static_vector<char, 23>;

        static auto CreateItemObject(const GameItem& item) -> boost::container::static_vector<char, 23>;
    };
}
