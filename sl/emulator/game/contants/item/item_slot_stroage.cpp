#include "item_slot_stroage.h"

namespace sunlight
{
    ItemSlotStorage::ItemSlotStorage(int32_t width, int32_t height)
        : _width(width)
        , _height(height)
    {
        if (_width <= 0 || _height <= 0)
        {
            throw std::runtime_error(fmt::format("invalid width, height. [{}, {}]",
                _width, _height));
        }

        _slots.resize(_width, std::vector<PtrNotNull<GameItem>>(_height));
    }

    bool ItemSlotStorage::Contains(const ItemSlotRange& range) const
    {
        if (range.x < 0 || range.xSize < 1 || range.x + range.xSize > _width)
        {
            return false;
        }

        if (range.y < 0 || range.ySize < 1 || range.y + range.ySize > _height)
        {
            return false;
        }

        return true;
    }

    bool ItemSlotStorage::HasEmptySlot(const ItemSlotRange& range) const
    {
        assert(Contains(range));

        for (int32_t i = range.x; i < range.x + range.xSize; ++i)
        {
            const std::vector<GameItem*>& container = _slots[i];

            for (int32_t j = range.y; j < range.y + range.ySize; ++j)
            {
                if (GameItem* item = container[j]; item)
                {
                    return false;
                }
            }
        }

        return true;
    }

    void ItemSlotStorage::Get(boost::unordered::unordered_flat_set<PtrNotNull<GameItem>>& result, const ItemSlotRange& range)
    {
        assert(Contains(range));

        for (int32_t i = range.x; i < range.x + range.xSize; ++i)
        {
            std::vector<GameItem*>& container = _slots[i];

            for (int32_t j = range.y; j < range.y + range.ySize; ++j)
            {
                if (GameItem* item = container[j]; item)
                {
                    (void)result.emplace(item);
                }
            }
        }
    }

    void ItemSlotStorage::Set(GameItem* item, const ItemSlotRange& range)
    {
        assert(Contains(range));

        for (int32_t i = range.x; i < range.x + range.xSize; ++i)
        {
            std::vector<GameItem*>& container = _slots[i];

            for (int32_t j = range.y; j < range.y + range.ySize; ++j)
            {
                container[j] = item;
            }
        }
    }

    void ItemSlotStorage::Clear()
    {
        for (auto& container : _slots)
        {
            container.clear();
        }
    }

    auto ItemSlotStorage::GetWidth() const -> int32_t
    {
        return _width;
    }

    auto ItemSlotStorage::GetHeight() const -> int32_t
    {
        return _height;
    }
}
