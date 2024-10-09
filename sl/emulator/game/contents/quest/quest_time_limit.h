#pragma once

namespace sunlight
{
    struct QuestTimeLimit
    {
        static constexpr const char* NAME = "quest_time_limit";

        int32_t flagIndex = 0;
        int64_t startTimePoint = 0;
        int64_t endTimePoint = 0;

        friend void to_json(nlohmann::json& j, const QuestTimeLimit& limit);
        friend void from_json(nlohmann::json& json, QuestTimeLimit& limit);
    };
}
