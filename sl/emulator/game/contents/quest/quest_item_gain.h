#pragma once

namespace sunlight
{
    struct QuestItemGain
    {
        static constexpr const char* NAME = "quest_item_gain";

        int32_t monsterId = 0;
        int32_t itemId = 0;
        int32_t probability = 0;
        int32_t itemMaxQuantity = 0;
        int32_t minKillCount = 0;
        int32_t killCount = 0;

        friend void to_json(nlohmann::json& j, const QuestItemGain& itemGain);
        friend void from_json(nlohmann::json& j, QuestItemGain& itemGain);
    };
}
