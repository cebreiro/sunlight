#pragma once
#include "sl/emulator/service/database/dto/character.h"
#include "sl/emulator/game/component/game_component.h"
#include "sl/emulator/game/contants/job/job_type.h"
#include "sl/emulator/game/contants/job/job.h"

namespace sunlight
{
    class GameDataProvideService;
}

namespace sunlight
{
    class PlayerJobComponent final : public GameComponent
    {
    public:
        PlayerJobComponent(const GameDataProvideService& dataProvider, const std::vector<db::dto::Character::Job>& jobs);

        bool HasJob(JobType type) const;

        auto Find(JobId id) -> Job*;
        auto Find(JobId id) const -> const Job*;
        auto Find(JobType type) -> Job*;
        auto Find(JobType type) const -> const Job*;

        auto GetMainJob() const -> const Job&;
        auto MutableMainJob() -> Job&;

    private:
        std::array<std::optional<Job>, static_cast<int32_t>(JobType::Count)> _jobs;
    };
}
