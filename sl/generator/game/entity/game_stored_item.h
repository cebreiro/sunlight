#pragma once
#include "sl/generator/game/entity/game_entity.h"

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
        GameStoredItem(GameEntityIdPublisher& idPublisher, game_entity_id_type ownerId, GameEntityType ownerType,
            const ItemData& data, int32_t quantity, int32_t groupId, int32_t page, int32_t price, int32_t fieldOffset);

        auto GetOwnerId() const -> game_entity_id_type;
        auto GetOwnerType() const -> GameEntityType;
        auto GetData() const -> const ItemData&;
        auto GetQuantity() const -> int32_t;
        auto GetGroupId() const -> int32_t;
        auto GetPage() const -> int32_t;
        auto GetPrice() const -> int32_t;
        auto GetFieldOffset() const -> int32_t;

    private:
        const ItemData& _data;

        game_entity_id_type _ownerId = {};
        GameEntityType _ownerType = {};

        int32_t _quantity = 0;
        int32_t _groupId = 0;
        int32_t _page = 0;
        int32_t _price = 0;
        int32_t _fieldOffset = 0;

    };
}
