#include "game_item.h"

namespace sunlight
{
    GameItem::GameItem(GameEntityIdPublisher& idPublisher, const ItemData& data, int32_t quantity)
        : GameEntity(idPublisher, TYPE)
        , _data(data)
        , _quantity(quantity)
    {
        assert(_quantity >= 1);
    }

    bool GameItem::HasUId() const
    {
        return _uid.has_value();
    }

    auto GameItem::GetData() const -> const ItemData&
    {
        return _data;
    }

    auto GameItem::GetUId() const -> const std::optional<int64_t>&
    {
        return _uid;
    }

    auto GameItem::GetQuantity() const -> int32_t
    {
        return _quantity;
    }

    void GameItem::SetUId(int64_t id)
    {
        assert(!_uid.has_value());

        _uid = id;
    }

    void GameItem::SetQuantity(int32_t quantity)
    {
        _quantity = quantity;
    }
}
