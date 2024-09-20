#include "player_job_system.h"

#include "sl/emulator/game/component/player_job_component.h"
#include "sl/emulator/game/data/sox/job_reference.h"
#include "sl/emulator/game/entity/game_player.h"
#include "sl/emulator/game/message/creator/game_player_message_creator.h"
#include "sl/emulator/game/system/game_repository_system.h"
#include "sl/emulator/game/zone/stage.h"
#include "sl/emulator/service/gamedata/gamedata_provide_service.h"
#include "sl/emulator/service/gamedata/exp/exp_data_provider.h"
#include "sl/emulator/service/gamedata/skill/skill_data_provider.h"

namespace sunlight
{
    PlayerJobSystem::PlayerJobSystem(const ServiceLocator& serviceLocator)
        : _serviceLocator(serviceLocator)
    {
    }

    void PlayerJobSystem::InitializeSubSystem(Stage& stage)
    {
        Add(stage.Get<GameRepositorySystem>());
    }

    bool PlayerJobSystem::Subscribe(Stage& stage)
    {
        (void)stage;

        return true;
    }

    auto PlayerJobSystem::GetName() const -> std::string_view
    {
        return "player_job_system";
    }

    auto PlayerJobSystem::GetClassId() const -> game_system_id_type
    {
        return GameSystem::GetClassId<PlayerJobSystem>();
    }

    void PlayerJobSystem::GainJobExp(GamePlayer& player, int32_t exp)
    {
        PlayerJobComponent& jobComponent = player.GetJobComponent();

        Job& job = jobComponent.MutableMainJob();

        if (job.GetLevel() >= job.GetData().maxJobLevel)
        {
            return;
        }

        const ExpDataProvider& expDataProvider = _serviceLocator.Get<GameDataProvideService>().GetExpDataProvider();
        const JobExpData* expData = expDataProvider.FindJobData(job.GetLevel() + (job.IsNovice() ? 1 : 20));

        if (!expData)
        {
            return;
        }

        job.SetExp(job.GetExp() + exp);
        player.Defer(GamePlayerMessageCreator::CreateJobExpGain(player, job.GetId(), exp));

        if (job.GetExp() >= expData->expMax)
        {
            job.SetExp(0);
            job.SetLevel(job.GetLevel() + 1);
            job.SetSkillPoint(job.GetSkillPoint() + 1);

            const std::vector<int32_t>& skills = GetJobGainSkills(job.GetId(), job.GetLevel());
            const auto transform = [cid = player.GetCId(), &job](int32_t skillId) -> req::SkillCreate
                {
                    return req::SkillCreate{
                        .cid = cid,
                        .id = skillId,
                        .job = static_cast<int32_t>(job.GetId()),
                        .level = 1,
                    };
                };

            Get<GameRepositorySystem>().SaveJobLevel(player, static_cast<int32_t>(job.GetId()), job.GetLevel(), job.GetSkillPoint(),
                std::ranges::to<std::vector>(skills | std::views::transform(transform)));

            player.Defer(GamePlayerMessageCreator::CreateJobExpLevelUp(player));

            for (int32_t skillId : skills)
            {
                player.Defer(GamePlayerMessageCreator::CreateJobSkillAdd(player, job.GetId(), skillId, 5));
            }
        }
        else
        {
            Get<GameRepositorySystem>().SaveJobExp(player, static_cast<int32_t>(job.GetId()), job.GetExp());
        }

        player.FlushDeferred();
    }

    auto PlayerJobSystem::GetJobGainSkills(JobId id, int32_t level) const -> std::vector<int32_t>
    {
        std::vector<int32_t> result;

        const SkillDataProvider& skillDataProvider = _serviceLocator.Get<GameDataProvideService>().GetSkillDataProvider();

        for (const PlayerSkillData& skillData : skillDataProvider.FindByJob(static_cast<int32_t>(id), level) | notnull::reference)
        {
            result.push_back(skillData.index);
        }

        const int32_t jobCommon = static_cast<int32_t>(id) % 1000 / 100 * 100;

        for (const PlayerSkillData& skillData : skillDataProvider.FindByJob(jobCommon, level) | notnull::reference)
        {
            result.push_back(skillData.index);
        }

        return result;
    }
}
