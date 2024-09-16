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

        _slots.resize(_height, std::vector<PtrNotNull<GameItem>>(_width));
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

        for (int32_t y = range.y; y < range.y + range.ySize; ++y)
        {
            const std::vector<GameItem*>& container = _slots[y];

            for (int32_t x = range.x; x < range.x + range.xSize; ++x)
            {
                if (GameItem* item = container[x]; item)
                {
                    return false;
                }
            }
        }

        return true;
    }

    auto ItemSlotStorage::FindEmpty(int32_t width, int32_t height) const -> std::optional<std::pair<int32_t, int32_t>>
    {
        if (width <= 0 || width > _width || height <= 0 || height > _height)
        {
            assert(false);

            return std::nullopt;
        }

        for (int32_t y = 0; y <= _height - height; ++y)
        {
            for (int32_t x = 0; x <= _width - width; ++x)
            {
                const ItemSlotRange range{
                    .x = x,
                    .y = y,
                    .xSize = width,
                    .ySize = height,
                };

                if (HasEmptySlot(range))
                {
                    return std::make_pair(x, y);
                }
            }
        }

        return std::nullopt;
    }

    void ItemSlotStorage::Get(boost::unordered::unordered_flat_set<PtrNotNull<GameItem>>& result, const ItemSlotRange& range)
    {
        assert(Contains(range));

        for (int32_t j = range.y; j < range.y + range.ySize; ++j)
        {
            std::vector<GameItem*>& container = _slots[j];

            for (int32_t i = range.x; i < range.x + range.xSize; ++i)
            {
                if (GameItem* item = container[i]; item)
                {
                    (void)result.emplace(item);
                }
            }
        }
    }

    void ItemSlotStorage::Set(GameItem* item, const ItemSlotRange& range)
    {
        assert(Contains(range));

        for (int32_t y = range.y; y < range.y + range.ySize; ++y)
        {
            std::vector<GameItem*>& container = _slots[y];

            for (int32_t x = range.x; x < range.x + range.xSize; ++x)
            {
                container[x] = item;
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
