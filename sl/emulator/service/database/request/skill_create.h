#pragma once

namespace sunlight::req
{
    struct SkillCreate
    {
        int64_t cid = 0;
        int32_t id = 0;
        int32_t job = 0;
        int32_t level = 1;

        friend void to_json(nlohmann::json& j, const SkillCreate& req);
    };
}
