#pragma once

namespace sunlight
{
    class GameItem;
}

namespace sunlight
{
    class ItemSlotStorage
    {
    public:
        ItemSlotStorage() = delete;

        ItemSlotStorage(int8_t width, int8_t height);

    private:
        int8_t _width = 0;
        int8_t _height = 0;

        std::vector<std::vector<PtrNotNull<const GameItem>>> _items;
    };
}
