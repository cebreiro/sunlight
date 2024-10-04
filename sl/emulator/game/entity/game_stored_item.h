#pragma once
#include "sl/emulator/game/entity/game_entity.h"

namespace sunlight
{
    class ItemData;
}

namespace sunlight
{
    class GameStoredItem final : public GameEntity
    {
    public:
        static constexpr GameEntityType TYPE = GameEntityType::ItemChild;

    public:
        GameStoredItem(GameEntityIdPublisher& idPublisher, const ItemData& data, int32_t quantity, int32_t groupId, int32_t price);

        auto GetData() const -> const ItemData&;
        auto GetQuantity() const -> int32_t;
        auto GetGroupId() const -> int32_t;
        auto GetPrice() const -> int32_t;

    private:
        const ItemData& _data;

        int32_t _quantity = 0;
        int32_t _groupId = 0;
        int32_t _price = 0;
    };
}
