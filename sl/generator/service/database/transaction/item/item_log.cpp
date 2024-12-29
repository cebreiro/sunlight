#include "item_log.h"

namespace sunlight::db
{
    void to_json(nlohmann::json& j, const ItemLogUpdatePosition& log)
    {
        j = nlohmann::json
        {
            { "type", log.type },
            { "id", log.id },
            { "posType", ToString(log.posType)},
            { "page", log.page },
            { "x", log.x },
            { "y", log.y },
        };
    }

    void to_json(nlohmann::json& j, const ItemLogUpdateQuantity& log)
    {
        j = nlohmann::json
        {
            { "type", log.type },
            { "id", log.id },
            { "quantity", log.quantity },
        };
    }

    void to_json(nlohmann::json& j, const ItemLogAdd& log)
    {
        j = nlohmann::json
        {
            { "type", log.type },
            { "id", log.id },
            { "cid", log.cid },
            { "itemId", log.itemId },
            { "quantity", log.quantity },
            { "posType", ToString(log.posType)},
            { "page", log.page },
            { "x", log.x },
            { "y", log.y },
        };
    }

    void to_json(nlohmann::json& j, const ItemLogRemove& log)
    {
        j = nlohmann::json
        {
            { "type", log.type },
            { "id", log.id },
        };
    }

    void to_json(nlohmann::json& j, const GoldChange& log)
    {
        j = nlohmann::json
        {
            { "type", log.type },
            { "cid", log.cid },
            { "gold", log.gold },
        };
    }

    void to_json(nlohmann::json& j, const ItemLogAccountStorageAdd& log)
    {
        j = nlohmann::json
        {
            { "type", log.type },
            { "id", log.id },
            { "aid", log.aid },
            { "itemId", log.itemId },
            { "quantity", log.quantity },
            { "page", log.page },
            { "x", log.x },
            { "y", log.y },
        };
    }

    void to_json(nlohmann::json& j, const ItemLogAccountStorageRemove& log)
    {
        j = nlohmann::json
        {
            { "type", log.type },
            { "id", log.id },
        };
    }

    void to_json(nlohmann::json& j, const ItemLog& log)
    {
        std::visit([&]<typename T>(const T& item)
            {
                if constexpr (std::is_same_v<T, ItemLogUpdatePosition>)
                {
                    to_json(j, item);
                }
                else if constexpr (std::is_same_v<T, ItemLogUpdateQuantity>)
                {
                    to_json(j, item);
                }
                else if constexpr (std::is_same_v<T, ItemLogAdd>)
                {
                    to_json(j, item);
                }
                else if constexpr (std::is_same_v<T, ItemLogRemove>)
                {
                    to_json(j, item);
                }
                else if constexpr (std::is_same_v<T, GoldChange>)
                {
                    to_json(j, item);
                }
                else if constexpr (std::is_same_v<T, ItemLogAccountStorageAdd>)
                {
                    to_json(j, item);
                }
                else if constexpr (std::is_same_v<T, ItemLogAccountStorageRemove>)
                {
                    to_json(j, item);
                }
                else
                {
                    static_assert(sizeof(T), "not implemented");
                }

            }, log);
    }
}
