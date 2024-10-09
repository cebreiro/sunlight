#include "job.h"

namespace sunlight
{
    Job::Job(const sox::JobReference& data, JobId id, int32_t level, int32_t exp, int32_t skillPoint)
        : _data(&data)
        , _id(id)
        , _level(level)
        , _exp(exp)
        , _skillPoint(skillPoint)
    {
    }

    bool Job::IsNovice() const
    {
        switch (_id)
        {
        case JobId::NoviceFighter:
        case JobId::NoviceRanger:
        case JobId::NoviceMagician:
        case JobId::NoviceArtisan:
            return true;
        default:;
        }

        return false;
    }

    auto Job::GetData() const -> const sox::JobReference&
    {
        assert(_data);

        return *_data;
    }

    auto Job::GetId() const -> JobId
    {
        return _id;
    }

    auto Job::GetLevel() const -> int32_t
    {
        return _level;
    }

    auto Job::GetExp() const -> int32_t
    {
        return _exp;
    }

    auto Job::GetSkillPoint() const -> int32_t
    {
        return _skillPoint;
    }

    void Job::SetLevel(int32_t level)
    {
        _level = level;
    }

    void Job::SetExp(int32_t exp)
    {
        _exp = exp;
    }

    void Job::SetSkillPoint(int32_t point)
    {
        _skillPoint = point;
    }
}
