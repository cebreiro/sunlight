#include "quest_time_limit.h"

namespace sunlight
{
    void to_json(nlohmann::json& j, const QuestTimeLimit& limit)
    {
        j = nlohmann::json
        {
            { "flagIndex", limit.flagIndex },
            { "startTimePoint", limit.startTimePoint },
            { "endTimePoint", limit.endTimePoint },
        };
    }

    void from_json(nlohmann::json& j, QuestTimeLimit& limit)
    {
		j["flagIndex"].get_to(limit.flagIndex);
		j["startTimePoint"].get_to(limit.startTimePoint);
		j["endTimePoint"].get_to(limit.endTimePoint);
    }
}
