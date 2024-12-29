#include "player_job_component.h"

#include "sl/generator/game/data/sox/job_reference.h"
#include "sl/generator/service/gamedata/gamedata_provide_service.h"

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

    bool PlayerJobComponent::AddJob(JobType type, const Job& job)
    {
        const int64_t index = static_cast<int64_t>(type);
        assert(index >= 0 && index < std::ssize(_jobs));

        std::optional<Job>& item = _jobs[index];
        if (item.has_value())
        {
            return false;
        }

        item = job;

        return true;
    }

    auto PlayerJobComponent::Find(JobId id) -> Job*
    {
        const auto iter = std::ranges::find_if(_jobs, [id](const std::optional<Job>& job) -> bool
            {
                return job.has_value() && job->GetId() == id;
            });

        return iter != _jobs.end() ? &iter->value() : nullptr;
    }

    auto PlayerJobComponent::Find(JobId id) const -> const Job*
    {
        const auto iter = std::ranges::find_if(_jobs, [id](const std::optional<Job>& job) -> bool
            {
                return job.has_value() && job->GetId() == id;
            });

        return iter != _jobs.end() ? &iter->value() : nullptr;
    }

    auto PlayerJobComponent::Find(JobType type) -> Job*
    {
        const int64_t index = static_cast<int64_t>(type);
        assert(index >= 0 && index < std::ssize(_jobs));

        std::optional<Job>& job = _jobs[index];

        return job.has_value() ? &(*job) : nullptr;
    }

    auto PlayerJobComponent::Find(JobType type) const -> const Job*
    {
        const int64_t index = static_cast<int64_t>(type);
        assert(index >= 0 && index < std::ssize(_jobs));

        const std::optional<Job>& job = _jobs[index];

        return job.has_value() ? &(*job) : nullptr;
    }

    auto PlayerJobComponent::GetMainJob() const -> const Job&
    {
        const Job* advanced = Find(JobType::Advanced);
        if (advanced)
        {
            return *advanced;
        }

        const Job* novice = Find(JobType::Novice);
        if (novice)
        {
            return *novice;
        }

        throw std::runtime_error("player has no job");
    }

    auto PlayerJobComponent::MutableMainJob() -> Job&
    {
        Job* advanced = Find(JobType::Advanced);
        if (advanced)
        {
            return *advanced;
        }

        Job* novice = Find(JobType::Novice);
        if (novice)
        {
            return *novice;
        }

        throw std::runtime_error("player has no job");
    }
}
