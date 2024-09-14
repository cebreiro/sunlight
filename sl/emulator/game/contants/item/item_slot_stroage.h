#pragma once
#include <boost/unordered/unordered_flat_set.hpp>

namespace sunlight
{
    class GameItem;
}

namespace sunlight
{
    struct ItemSlotRange
    {
        int32_t x = 0;
        int32_t y = 0;
        int32_t xSize = 1;
        int32_t ySize = 1;
    };

    class ItemSlotStorage
    {
    public:
        ItemSlotStorage() = delete;
        ItemSlotStorage(int32_t width, int32_t height);

        bool Contains(const ItemSlotRange& range) const;
        bool HasEmptySlot(const ItemSlotRange& range) const;

        void Get(boost::unordered::unordered_flat_set<PtrNotNull<GameItem>>& result, const ItemSlotRange& range);
        void Set(GameItem* item, const ItemSlotRange& range);

        void Clear();

        auto GetWidth() const -> int32_t;
        auto GetHeight() const -> int32_t;

    private:
        int32_t _width = 0;
        int32_t _height = 0;

        std::vector<std::vector<GameItem*>> _slots;
    };
}
