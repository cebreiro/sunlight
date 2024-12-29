#pragma once
#include <boost/container/flat_map.hpp>

#include "sl/generator/game/game_constant.h"
#include "sl/generator/game/component/game_component.h"
#include "sl/generator/game/entity/game_entity_id_type.h"

namespace sunlight
{
    class StreetVendorHostComponent final : public GameComponent
    {
    public:
        bool IsOpen() const;
        bool IsValid(int32_t page) const;
        bool IsDisplayedItemPage(int32_t page) const;

        bool IsEmpty() const;
        bool IsItemPriceConfiguredAll() const;

        void AddStoredItem(int32_t page, game_entity_id_type id);
        void RemoveStoredItem(int32_t page);

        auto GetItemPrice(int32_t page) const -> int32_t;
        auto GetStoredItem(int32_t page) const -> std::optional<game_entity_id_type>;

        void SetOpen(bool value);
        void SetItem(int32_t page);
        void SetItemPrice(int32_t page, int32_t price);

        inline auto GetStoredItems() const;

    private:
        bool _open = false;

        std::array<std::optional<int32_t>, GameConstant::MAX_STREET_VENDOR_PAGE_SIZE> _prices = {};

        boost::container::small_flat_map<int32_t, game_entity_id_type, GameConstant::MAX_STREET_VENDOR_STORED_ITEM_SIZE> _storedItems;
    };

    inline auto StreetVendorHostComponent::GetStoredItems() const
    {
        return _storedItems | std::views::values;
    }
}
