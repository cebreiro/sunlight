#include "street_vendor_host_component.h"

namespace sunlight
{
    bool StreetVendorHostComponent::IsOpen() const
    {
        return _open;
    }

    bool StreetVendorHostComponent::IsValid(int32_t page) const
    {
        if (page < 0 || page >= std::ssize(_prices))
        {
            return false;
        }

        return true;
    }

    bool StreetVendorHostComponent::IsEmpty() const
    {
        return std::ranges::all_of(_prices, [](std::optional<int32_t> value)
            {
                return !value.has_value();
            });
    }

    bool StreetVendorHostComponent::IsItemPriceConfiguredAll() const
    {
        int32_t nullCount = 0;

        for (const std::optional<int32_t> value : _prices)
        {
            if (!value.has_value())
            {
                ++nullCount;

                continue;
            }

            if (*value <= 0)
            {
                return false;
            }
        }

        if (nullCount >= std::ssize(_prices))
        {
            return false;
        }

        return true;
    }

    auto StreetVendorHostComponent::GetItemPrice(int32_t page) const -> int32_t
    {
        if (!IsValid(page))
        {
            return 0;
        }

        return _prices[page].value_or(0);
    }

    auto StreetVendorHostComponent::GetStoredItem(int64_t slot) const -> std::optional<game_entity_id_type>
    {
        assert(slot >= 0 && slot < std::ssize(_storedItems));

        return _storedItems[slot];
    }

    void StreetVendorHostComponent::SetItem(int32_t page)
    {
        assert(IsValid(page));

        _prices[page].emplace(0);
    }

    void StreetVendorHostComponent::SetItemPrice(int32_t page, int32_t price)
    {
        assert(IsValid(page));

        _prices[page] = price;
    }

    auto StreetVendorHostComponent::GetEmptyStoredItemSlot() const -> std::optional<int32_t>
    {
        for (int32_t i = 0; i < std::ssize(_storedItems); ++i)
        {
            if (!_storedItems[i].has_value())
            {
                return i;
            }
        }

        return std::nullopt;
    }

    void StreetVendorHostComponent::AddStoredItem(int64_t slot, game_entity_id_type id)
    {
        assert(slot >= 0 && slot < std::ssize(_storedItems));
        assert(!_storedItems[slot].has_value());

        _storedItems[slot] = id;
    }

    void StreetVendorHostComponent::RemoveStoredItem(const game_entity_id_type id)
    {
        for (int32_t i = 0; i < std::ssize(_storedItems); ++i)
        {
            std::optional<game_entity_id_type>& element = _storedItems[i];
            if (element && *element == id)
            {
                element.reset();

                return;
            }
        }

        assert(false);
    }

    void StreetVendorHostComponent::SetOpen(bool value)
    {
        _open = value;
    }
}
