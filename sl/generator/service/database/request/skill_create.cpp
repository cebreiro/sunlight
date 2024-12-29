#include "skill_create.h"

namespace sunlight::req
{
    void to_json(nlohmann::json& j, const SkillCreate& req)
    {
        j = nlohmann::json
        {
            { "cid", req.cid },
            { "id", req.id },
            { "job", req.job },
            { "level", req.level }
        };
    }
}
