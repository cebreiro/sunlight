#include "quest_item_gain.h"

namespace sunlight
{
    void to_json(nlohmann::json& j, const QuestItemGain& itemGain)
    {
        j = nlohmann::json
        {
            { "monsterId", itemGain.monsterId },
            { "itemId", itemGain.itemId },
            { "probability", itemGain.probability },
            { "itemMaxQuantity", itemGain.itemMaxQuantity },
            { "minKillCount", itemGain.minKillCount },
            { "killCount", itemGain.killCount },
        };
    }

    void from_json(nlohmann::json& j, QuestItemGain& itemGain)
    {
        j["monsterId"].get_to(itemGain.monsterId);
        j["itemId"].get_to(itemGain.itemId);
        j["probability"].get_to(itemGain.probability);
        j["itemMaxQuantity"].get_to(itemGain.itemMaxQuantity);
        j["killCount"].get_to(itemGain.killCount);
    }
}
