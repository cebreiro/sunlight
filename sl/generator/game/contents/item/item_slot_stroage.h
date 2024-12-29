#pragma once
#include "sl/generator/game/contents/item/item_slot_storage_base.h"

namespace sunlight
{
    class GameItem;
}

namespace sunlight
{

    class ItemSlotStorage final : public ItemSlotStorageBase<GameItem*>
    {
    public:
        ItemSlotStorage(int32_t width, int32_t height);
    };
}
