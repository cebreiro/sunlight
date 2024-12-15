#pragma once
#include "sl/generator/service/database/enum/item_pos_type.h"

namespace sunlight::db
{
    ENUM_CLASS(ItemLogType, int32_t,
        (UpdatePosition, 1)
        (UpdateQuantity, 2)
        (Add, 3)
        (Remove, 4)
        (GoldChange, 5)
        (AccountStorageAdd, 6)
        (AccountStorageRemove, 7)
    )

    struct ItemLogUpdatePosition
    {
        static constexpr auto type = ItemLogType::UpdatePosition;

        int64_t id = 0;
        ItemPosType posType = {};
        int8_t page = -1;
        int8_t x = -1;
        int8_t y = -1;

        friend void to_json(nlohmann::json& j, const ItemLogUpdatePosition& log);
    };

    struct ItemLogUpdateQuantity
    {
        static constexpr auto type = ItemLogType::UpdateQuantity;

        int64_t id = 0;
        int32_t quantity = 0;

        friend void to_json(nlohmann::json& j, const ItemLogUpdateQuantity& log);
    };

    struct ItemLogAdd
    {
        static constexpr auto type = ItemLogType::Add;

        int64_t id = 0;
        int64_t cid = 0;
        int32_t itemId = 0;
        int32_t quantity = 0;
        ItemPosType posType = {};
        int8_t page = -1;
        int8_t x = -1;
        int8_t y = -1;

        friend void to_json(nlohmann::json& j, const ItemLogAdd& log);
    };

    struct ItemLogRemove
    {
        static constexpr auto type = ItemLogType::Remove;

        int64_t id = 0;

        friend void to_json(nlohmann::json& j, const ItemLogRemove& log);
    };

    struct GoldChange
    {
        static constexpr auto type = ItemLogType::GoldChange;

        int64_t cid = 0;
        int32_t gold = 0;

        friend void to_json(nlohmann::json& j, const GoldChange& log);
    };

    struct ItemLogAccountStorageAdd
    {
        static constexpr auto type = ItemLogType::AccountStorageAdd;

        int64_t id = 0;
        int64_t aid = 0;
        int32_t itemId = 0;
        int32_t quantity = 0;
        int8_t page = -1;
        int8_t x = -1;
        int8_t y = -1;

        friend void to_json(nlohmann::json& j, const ItemLogAccountStorageAdd& log);
    };

    struct ItemLogAccountStorageRemove
    {
        static constexpr auto type = ItemLogType::AccountStorageRemove;

        int64_t id = 0;

        friend void to_json(nlohmann::json& j, const ItemLogAccountStorageRemove& log);
    };

    using ItemLog = std::variant<
        ItemLogUpdatePosition, ItemLogUpdateQuantity, ItemLogAdd, ItemLogRemove, GoldChange,
        ItemLogAccountStorageAdd, ItemLogAccountStorageRemove>;

    void to_json(nlohmann::json& j, const ItemLog& log);
}
