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

    bool StreetVendorHostComponent::IsDisplayedItemPage(int32_t page) const
    {
        return _storedItems.contains(page);
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

    auto StreetVendorHostComponent::GetStoredItem(int32_t page) const -> std::optional<game_entity_id_type>
    {
        const auto iter = _storedItems.find(page);

        return iter != _storedItems.end() ? iter->second : std::optional<game_entity_id_type>();
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

    void StreetVendorHostComponent::AddStoredItem(int32_t page, game_entity_id_type id)
    {
        assert(!_storedItems.contains(page));

        _storedItems.emplace(page, id);
    }

    void StreetVendorHostComponent::RemoveStoredItem(int32_t page)
    {
        [[maybe_unused]]
        const bool removed = _storedItems.erase(page);
        assert(removed);
    }

    void StreetVendorHostComponent::SetOpen(bool value)
    {
        _open = value;
    }
}
