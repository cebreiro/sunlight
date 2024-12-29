#pragma once
#include "sl/generator/game/entity/game_entity.h"

namespace sunlight
{
    class ItemData;
}

namespace sunlight
{
    class GameItem final : public GameEntity
    {
    public:
        static constexpr GameEntityType TYPE = GameEntityType::Item;

    public:
        GameItem(GameEntityIdPublisher& idPublisher, const ItemData& data, int32_t quantity);

        bool HasUId() const;

        auto GetData() const -> const ItemData&;
        auto GetUId() const -> const std::optional<int64_t>&;
        auto GetQuantity() const -> int32_t;

        void SetUId(int64_t id);
        void SetQuantity(int32_t quantity);

    private:
        const ItemData& _data;

        std::optional<int64_t> _uid = std::nullopt;
        int32_t _quantity = 0;
    };
}
