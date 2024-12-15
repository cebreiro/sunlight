#pragma once
#include "sl/generator/game/contents/job/job_id.h"

namespace sunlight::sox
{
    struct JobReference;
}

namespace sunlight
{
    class Job
    {
    public:
        Job() = default;

        Job(const sox::JobReference& data, JobId id, int32_t level, int32_t exp, int32_t skillPoint);

        bool IsNovice() const;

        auto GetData() const -> const sox::JobReference&;
        auto GetId() const -> JobId;
        auto GetLevel() const -> int32_t;
        auto GetExp() const -> int32_t;
        auto GetSkillPoint() const -> int32_t;

        void SetLevel(int32_t level);
        void SetExp(int32_t exp);
        void SetSkillPoint(int32_t point);

    private:
        PtrNotNull<const sox::JobReference> _data = nullptr;

        JobId _id =  JobId::None;
        int32_t _level = 0;
        int32_t _exp = 0;
        int32_t _skillPoint = 0;
    };
}
