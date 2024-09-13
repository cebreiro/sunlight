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

        auto GetIf(JobType type) const -> const Job*;

    private:
        std::array<std::optional<Job>, static_cast<int32_t>(JobType::Count)> _jobs;
    };
}
