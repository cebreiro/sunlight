#pragma once
#include <boost/unordered/unordered_flat_set.hpp>

namespace sunlight
{
    struct ItemSlotRange
    {
        int32_t x = 0;
        int32_t y = 0;
        int32_t xSize = 1;
        int32_t ySize = 1;
    };

    template <typename T>
    class ItemSlotStorageBase
    {
        template <typename U>
        friend class ItemSlotStorageBase;

    public:
        ItemSlotStorageBase(int32_t width, int32_t height);

        bool Contains(const ItemSlotRange& range) const;
        bool HasEmptySlot(const ItemSlotRange& range) const;

        auto FindEmpty(int32_t width, int32_t height) const -> std::optional<std::pair<int32_t, int32_t>>;

        auto Get(int32_t x, int32_t y) -> T;
        auto Get(int32_t x, int32_t y) const -> const T;
        void Get(boost::unordered::unordered_flat_set<T>& result, const ItemSlotRange& range);

        void Set(T item, const ItemSlotRange& range);

        void Clear();

        auto GetMask() const -> ItemSlotStorageBase<bool>;

        auto GetWidth() const -> int32_t;
        auto GetHeight() const -> int32_t;
        auto GetLoadFactor() const -> double;

        auto GetDebugString() const -> std::string;

    private:
        auto CalculateIndex(int32_t x, int32_t y) const -> int32_t;

    private:
        int32_t _width = 0;
        int32_t _height = 0;

        std::vector<T> _slots;

        int32_t _used = 0;
    };

    template <typename T>
    ItemSlotStorageBase<T>::ItemSlotStorageBase(int32_t width, int32_t height)
        : _width(width)
        , _height(height)
    {
        if (_width <= 0 || _height <= 0)
        {
            throw std::runtime_error(fmt::format("invalid width, height. [{}, {}]",
                _width, _height));
        }

        _slots.resize(_width * _height);
    }

    template <typename T>
    bool ItemSlotStorageBase<T>::Contains(const ItemSlotRange& range) const
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

    template <typename T>
    bool ItemSlotStorageBase<T>::HasEmptySlot(const ItemSlotRange& range) const
    {
        assert(Contains(range));

        for (int32_t y = range.y; y < range.y + range.ySize; ++y)
        {
            for (int32_t x = range.x; x < range.x + range.xSize; ++x)
            {
                if (T item = _slots[CalculateIndex(x, y)]; item)
                {
                    return false;
                }
            }
        }

        return true;
    }

    template <typename T>
    auto ItemSlotStorageBase<T>::FindEmpty(int32_t width, int32_t height) const -> std::optional<std::pair<int32_t, int32_t>>
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

    template <typename T>
    auto ItemSlotStorageBase<T>::Get(int32_t x, int32_t y) -> T
    {
        assert(Contains(ItemSlotRange{
            .x = x,
            .y = y,
            .xSize = 1,
            .ySize = 1
            }));

        return _slots[CalculateIndex(x, y)];
    }

    template <typename T>
    auto ItemSlotStorageBase<T>::Get(int32_t x, int32_t y) const -> const T
    {
        assert(Contains(ItemSlotRange{
            .x = x,
            .y = y,
            .xSize = 1,
            .ySize = 1
            }));

        return _slots[CalculateIndex(x, y)];
    }

    template <typename T>
    void ItemSlotStorageBase<T>::Get(boost::unordered::unordered_flat_set<T>& result, const ItemSlotRange& range)
    {
        assert(Contains(range));

        for (int32_t j = range.y; j < range.y + range.ySize; ++j)
        {
            for (int32_t i = range.x; i < range.x + range.xSize; ++i)
            {
                if (T item = _slots[CalculateIndex(i, j)]; item)
                {
                    (void)result.emplace(item);
                }
            }
        }
    }

    template <typename T>
    void ItemSlotStorageBase<T>::Set(T item, const ItemSlotRange& range)
    {
        assert(Contains(range));

        for (int32_t y = range.y; y < range.y + range.ySize; ++y)
        {
            for (int32_t x = range.x; x < range.x + range.xSize; ++x)
            {
                const int32_t index = CalculateIndex(x, y);
                
                T prev = _slots[index];
                _slots[index] = item;

                if (prev != item)
                {
                    if (item)
                    {
                        ++_used;
                    }
                    else
                    {
                        --_used;
                    }
                }
            }
        }

        assert(_used >= 0 && _used <= _width * _height);
    }

    template <typename T>
    void ItemSlotStorageBase<T>::Clear()
    {
        for (T& slot : _slots)
        {
            slot = T();
        }

        _used = 0;
    }

    template <typename T>
    auto ItemSlotStorageBase<T>::GetWidth() const -> int32_t
    {
        return _width;
    }

    template <typename T>
    auto ItemSlotStorageBase<T>::GetHeight() const -> int32_t
    {
        return _height;
    }

    template <typename T>
    auto ItemSlotStorageBase<T>::GetLoadFactor() const -> double
    {
        return static_cast<double>(_used) / static_cast<double>(_width * _height);
    }

    template <typename T>
    auto ItemSlotStorageBase<T>::GetMask() const -> ItemSlotStorageBase<bool>
    {
        ItemSlotStorageBase<bool> result(_width, _height);
        result._used = _used;

        for (int64_t i = 0; i < (_width * _height); ++i)
        {
            result._slots[i] = _slots[i] ? true : false;
        }

        return result;
    }

    template <typename T>
    auto ItemSlotStorageBase<T>::GetDebugString() const -> std::string
    {
        std::unordered_map<T, char> typeMap;

        char nextCharacter = 'A';
        std::ostringstream oss;

        oss << "\n--------------------------------------------------\n";

        for (int32_t y = 0; y < _height; ++y)
        {
            for (int32_t x = 0; x < _width; ++x)
            {
                if (T item = _slots[CalculateIndex(x, y)]; item)
                {
                    const char value = [&]() -> char
                        {
                            const auto iter = typeMap.find(item);
                            if (iter == typeMap.end())
                            {
                                return typeMap[item] = nextCharacter++;
                            }

                            return iter->second;

                        }();

                    oss << value;
                }
                else
                {
                    oss << '-';
                }

                oss << ' ';
            }

            oss << '\n';
        }

        oss << "--------------------------------------------------\n";

        return oss.str();
    }

    template <typename T>
    auto ItemSlotStorageBase<T>::CalculateIndex(int32_t x, int32_t y) const -> int32_t
    {
        assert(_width > 0);
        assert(_height > 0);

        const int32_t result = x + (y * _width);
        assert(result >= 0 && result < _width * _height);

        return result;
    }
}
