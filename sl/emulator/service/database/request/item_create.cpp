#include "item_create.h"

namespace sunlight::req
{
    void to_json(nlohmann::json& j, const ItemCreate& req)
    {
        j = nlohmann::json
        {
            { "id", req.id },
            { "cid", req.cid },
            { "itemId", req.itemId },
            { "quantity", req.quantity },
            { "posType", ToString(req.posType) },
            { "page", req.page },
            { "x", req.x },
            { "y", req.y },
        };
    }
}
