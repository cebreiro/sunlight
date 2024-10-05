#include "player_job_system.h"

#include "sl/emulator/game/component/player_job_component.h"
#include "sl/emulator/game/component/player_skill_component.h"
#include "sl/emulator/game/data/sox/job_reference.h"
#include "sl/emulator/game/entity/game_player.h"
#include "sl/emulator/game/message/zone_message.h"
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

    bool PlayerJobSystem::Promote(GamePlayer& player, JobId advanced)
    {
        PlayerJobComponent& jobComponent = player.GetJobComponent();

        if (jobComponent.HasJob(JobType::Advanced))
        {
            return false;
        }

        Job& job = jobComponent.MutableMainJob();
        if (!job.IsNovice())
        {
            return false;
        }

        if (job.GetLevel() < job.GetData().maxJobLevel)
        {
            return false;
        }

        if (!IsPromotable(job.GetId(), advanced))
        {
            return false;
        }

        const GameDataProvideService& gameDataProvider = _serviceLocator.Get<GameDataProvideService>();
        const sox::JobReference* newJobData = gameDataProvider.Get<sox::JobReferenceTable>().Find(static_cast<int32_t>(advanced));

        if (!newJobData)
        {
            return false;
        }

        const Job newJob(*newJobData, advanced, 1, 0, 0);
        (void)jobComponent.AddJob(JobType::Advanced, newJob);

        std::vector<int32_t> skills = GetJobGainSkills(newJob.GetId(), newJob.GetLevel());

        PlayerSkillComponent& skillComponent = player.GetSkillComponent();

        if (!skills.empty())
        {
            const SkillDataProvider& skillDataProvider = gameDataProvider.GetSkillDataProvider();

            for (auto iter = skills.begin(); iter != skills.end(); )
            {
                const int32_t skillId = *iter;

                if (const PlayerSkillData* skillData = skillDataProvider.FindPlayerSkill(skillId);
                    skillData)
                {
                    if (skillComponent.AddSkill(PlayerSkill(advanced, skillData)))
                    {
                        player.Defer(GamePlayerMessageCreator::CreateJobSkillAdd(player, newJob.GetId(), skillId, 0));

                        ++iter;

                        continue;
                    }
                }

                iter = skills.erase(iter);
            }
        }

        Get<GameRepositorySystem>().SaveNewJob(player, static_cast<int32_t>(newJob.GetId()),
            static_cast<int32_t>(JobType::Advanced), newJob.GetLevel(), newJob.GetSkillPoint(),
            std::ranges::to<std::vector>(skills | std::views::transform([cid = player.GetCId(), &newJob](int32_t skillId) -> req::SkillCreate
                {
                    return req::SkillCreate{
                        .cid = cid,
                        .id = skillId,
                        .job = static_cast<int32_t>(newJob.GetId()),
                        .level = 1,
                    };
                })));

        player.Defer(GamePlayerMessageCreator::CreateJobPromotion(player, advanced));
        player.FlushDeferred();

        // TODO: update stat

        return true;
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

            std::vector<int32_t> skills = GetJobGainSkills(job.GetId(), job.GetLevel());

            if (!skills.empty())
            {
                const GameDataProvideService& gameDataProvider = _serviceLocator.Get<GameDataProvideService>();
                const SkillDataProvider& skillDataProvider = gameDataProvider.GetSkillDataProvider();
                PlayerSkillComponent& skillComponent = player.GetSkillComponent();

                for (auto iter = skills.begin(); iter != skills.end(); )
                {
                    const int32_t skillId = *iter;

                    if (const PlayerSkillData* skillData = skillDataProvider.FindPlayerSkill(skillId);
                        skillData)
                    {
                        if (skillComponent.AddSkill(PlayerSkill(job.GetId(), skillData)))
                        {
                            player.Defer(GamePlayerMessageCreator::CreateJobSkillAdd(player, job.GetId(), skillId, 0));

                            ++iter;

                            continue;
                        }
                    }

                    iter = skills.erase(iter);
                }
            }

            Get<GameRepositorySystem>().SaveJobLevel(player, static_cast<int32_t>(job.GetId()), job.GetLevel(), job.GetSkillPoint(),
                std::ranges::to<std::vector>(skills | std::views::transform([cid = player.GetCId(), &job](int32_t skillId) -> req::SkillCreate
                    {
                        return req::SkillCreate{
                            .cid = cid,
                            .id = skillId,
                            .job = static_cast<int32_t>(job.GetId()),
                            .level = 1,
                        };
                    })));

            player.Defer(GamePlayerMessageCreator::CreateJobExpLevelUp(player));
        }
        else
        {
            Get<GameRepositorySystem>().SaveJobExp(player, static_cast<int32_t>(job.GetId()), job.GetExp());
        }

        player.FlushDeferred();
    }

    void PlayerJobSystem::OnSkillLevelSet(const ZoneMessage& message)
    {
        SlPacketReader& reader = message.reader;
        GamePlayer& player = message.player;

        const int32_t skillId = reader.Read<int32_t>();
        const int32_t newLevel = reader.Read<int32_t>();

        PlayerJobComponent& jobComponent = player.GetJobComponent();
        PlayerSkillComponent& skillComponent = player.GetSkillComponent();

        std::string error;

        do
        {
            PlayerSkill* skill = skillComponent.FindSkill(skillId);
            if (!skill)
            {
                error = fmt::format("fail to find skill_id");

                break;
            }

            const int32_t skillLevel = skill->GetBaseLevel();
            const int32_t maxLevel = skill->GetData().maxLevel;

            if (skillLevel >= maxLevel || newLevel > maxLevel || skillLevel >= newLevel)
            {
                error = fmt::format("invalid skill level condition");

                break;
            }

            Job* job = jobComponent.Find(skill->GetJobId());
            if (!job)
            {
                error = fmt::format("fail to find skill's job");

                break;
            }

            const int32_t usedSkillPoint = newLevel - skillLevel;

            if (usedSkillPoint > job->GetSkillPoint())
            {
                error = fmt::format("lack of skill point");

                break;
            }

            skill->SetBaseLevel(newLevel);
            job->SetSkillPoint(job->GetSkillPoint() - usedSkillPoint);

            assert(job->GetSkillPoint() >= 0);

            Get<GameRepositorySystem>().SaveSkillLevel(player,
                static_cast<int32_t>(job->GetId()), job->GetSkillPoint(), skill->GetId(), newLevel);

            player.Send(GamePlayerMessageCreator::CreateJobSkillPointChange(player, job->GetId(), job->GetSkillPoint(), false));

            return;
            
        } while (false);

        SUNLIGHT_LOG_INFO(_serviceLocator,
            fmt::format("[{}] fail to handle. player: {}, error: {}",
                GetName(), player.GetCId(), error));
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

    bool PlayerJobSystem::IsPromotable(JobId novice, JobId advanced)
    {
        switch (advanced)
        {
        case JobId::SwordWarrior:
        case JobId::Berserker:
        case JobId::Dragoon:
        case JobId::MartialArtist:
            return novice == JobId::NoviceFighter;
        case JobId::Archer:
        case JobId::Gunner:
        case JobId::Agent:
        case JobId::TreasureHunter:
            return novice == JobId::NoviceRanger;
        case JobId::Mage:
        case JobId::Healer:
        case JobId::Mystic:
        case JobId::Engineer:
            return novice == JobId::NoviceMagician;
        case JobId::WeaponSmith:
        case JobId::Designer:
        case JobId::Cook:
        case JobId::Chemist:
            return novice == JobId::NoviceArtisan;
        }

        return false;
    }
}
