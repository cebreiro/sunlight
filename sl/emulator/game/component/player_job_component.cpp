#include "player_job_component.h"

#include "sl/emulator/game/data/sox/job_reference.h"
#include "sl/emulator/service/gamedata/gamedata_provide_service.h"

namespace sunlight
{
    PlayerJobComponent::PlayerJobComponent(const GameDataProvideService& dataProvider, const std::vector<db::dto::Character::Job>& jobs)
    {
        const sox::JobReferenceTable& jobReferenceTable = dataProvider.Get<sox::JobReferenceTable>();

        for (const db::dto::Character::Job& dto : jobs)
        {
            if (dto.type < 0 || dto.type >= std::ssize(_jobs))
            {
                throw std::runtime_error(fmt::format("invalid job type. job_type: {}", dto.type));
            }

            std::optional<Job>& item = _jobs[dto.type];;
            if (item.has_value())
            {
                throw std::runtime_error(fmt::format("duplicated job. job_type: {}", dto.type));
            }

            const sox::JobReference* soxData = jobReferenceTable.Find(dto.id);
            if (!soxData)
            {
                throw std::runtime_error(fmt::format("fail to find job data from sox table. job_id: {}", dto.id));
            }

            item.emplace(*soxData, static_cast<JobId>(dto.id), dto.level, dto.exp, dto.skillPoint);
        }
    }

    bool PlayerJobComponent::HasJob(JobType type) const
    {
        const int64_t index = static_cast<int64_t>(type);
        assert(index >= 0 && index < std::ssize(_jobs));

        return _jobs[index].has_value();
    }

    auto PlayerJobComponent::GetIf(JobType type) const -> const Job*
    {
        const int64_t index = static_cast<int64_t>(type);
        assert(index >= 0 && index < std::ssize(_jobs));

        const std::optional<Job>& job = _jobs[index];

        return job.has_value() ? &(*job) : nullptr;
    }
}
