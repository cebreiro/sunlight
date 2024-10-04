#include "game_stored_item.h"

namespace sunlight
{
    GameStoredItem::GameStoredItem(GameEntityIdPublisher& idPublisher, const ItemData& data, int32_t quantity, int32_t groupId, int32_t price)
        : GameEntity(idPublisher, TYPE)
        , _data(data)
        , _quantity(quantity)
        , _groupId(groupId)
        , _price(price)
    {
    }

    auto GameStoredItem::GetData() const -> const ItemData&
    {
        return _data;
    }

    auto GameStoredItem::GetQuantity() const -> int32_t
    {
        return _quantity;
    }

    auto GameStoredItem::GetGroupId() const -> int32_t
    {
        return _groupId;
    }

    auto GameStoredItem::GetPrice() const -> int32_t
    {
        return _price;
    }
}
