#include "game_stored_item.h"

namespace sunlight
{
    GameStoredItem::GameStoredItem(GameEntityIdPublisher& idPublisher, game_entity_id_type ownerId, GameEntityType ownerType,
        const ItemData& data, int32_t quantity, int32_t groupId, int32_t page, int32_t price, int32_t fieldOffset)
        : GameEntity(idPublisher, TYPE)
        , _data(data)
        , _ownerId(ownerId)
        , _ownerType(ownerType)
        , _quantity(quantity)
        , _groupId(groupId)
        , _page(page)
        , _price(price)
        , _fieldOffset(fieldOffset)
    {
    }

    auto GameStoredItem::GetOwnerId() const -> game_entity_id_type
    {
        return _ownerId;
    }

    auto GameStoredItem::GetOwnerType() const -> GameEntityType
    {
        return _ownerType;
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

    auto GameStoredItem::GetPage() const -> int32_t
    {
        return _page;
    }

    auto GameStoredItem::GetPrice() const -> int32_t
    {
        return _price;
    }

    auto GameStoredItem::GetFieldOffset() const -> int32_t
    {
        return _fieldOffset;
    }
}
