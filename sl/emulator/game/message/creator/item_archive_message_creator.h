#pragma once
#include <boost/container/static_vector.hpp>
#include "sl/emulator/game/message/zone_message_type.h"

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
        static auto CreateInventoryItemAdd(const GamePlayer& player, const GameItem& item) -> Buffer;

        static auto CreateArchiveResult(const GamePlayer& player, bool result, ZoneMessageType archiveMessage) -> Buffer;

    private:
        using item_object_buffer_type = boost::container::static_vector<char, 23>;

        static auto CreateItemObject(const GameItem& item) -> boost::container::static_vector<char, 23>;
    };
}
