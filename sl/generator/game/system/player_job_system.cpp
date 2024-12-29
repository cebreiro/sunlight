#include "player_job_system.h"

#include "sl/generator/game/game_constant.h"
#include "sl/generator/game/component/player_job_component.h"
#include "sl/generator/game/component/player_skill_component.h"
#include "sl/generator/game/data/sox/job_reference.h"
#include "sl/generator/game/entity/game_player.h"
#include "sl/generator/game/message/zone_message.h"
#include "sl/generator/game/message/creator/game_player_message_creator.h"
#include "sl/generator/game/message/creator/item_archive_message_creator.h"
#include "sl/generator/game/system/entity_view_range_system.h"
#include "sl/generator/game/system/event_bubbling_system.h"
#include "sl/generator/game/system/player_stat_system.h"
#include "sl/generator/game/system/event_bubbling/player_event_bubbling.h"
#include "sl/generator/game/zone/stage.h"
#include "sl/generator/game/zone/service/game_repository_service.h"
#include "sl/generator/game/zone/service/zone_execution_service.h"
#include "sl/generator/service/gamedata/gamedata_provide_service.h"
#include "sl/generator/service/gamedata/exp/exp_data_provider.h"
#include "sl/generator/service/gamedata/skill/skill_data_provider.h"

namespace sunlight
{
    PlayerJobSystem::PlayerJobSystem(const ServiceLocator& serviceLocator, int32_t stageId)
        : _serviceLocator(serviceLocator)
        , _stageId(stageId)
    {
    }

    void PlayerJobSystem::InitializeSubSystem(Stage& stage)
    {
        Add(stage.Get<PlayerStatSystem>());
        Add(stage.Get<EntityViewRangeSystem>());
        Add(stage.Get<EventBubblingSystem>());
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

        _serviceLocator.Get<GameRepositoryService>().SaveNewJob(player, static_cast<int32_t>(newJob.GetId()),
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

        Get<PlayerStatSystem>().UpdateJobStat(player);

        return true;
    }

    bool PlayerJobSystem::AddSkill(GamePlayer& player, JobId jobId, int32_t skillId)
    {
        const GameDataProvideService& gameDataProvider = _serviceLocator.Get<GameDataProvideService>();

        if (jobId != JobId::Any)
        {
            if (const sox::JobReference* jobData = gameDataProvider.Get<sox::JobReferenceTable>().Find(static_cast<int32_t>(jobId));
                !jobData)
            {
                return false;
            }
        }

        const PlayerSkillData* skillData = gameDataProvider.GetSkillDataProvider().FindPlayerSkill(skillId);
        if (!skillData)
        {
            return false;
        }

        if (std::ranges::all_of(skillData->jobs, [requested = static_cast<int32_t>(jobId)](int32_t skillJob) -> bool
            {
                return skillJob != requested;
            }))
        {
            return false;
        }

        PlayerSkillComponent& skillComponent = player.GetSkillComponent();

        if (!skillComponent.AddSkill(PlayerSkill(jobId, skillData)))
        {
            return false;
        }

        _serviceLocator.Get<GameRepositoryService>().SaveNewSkill(player, static_cast<int32_t>(jobId), skillData->index, 1);

        player.Send(GamePlayerMessageCreator::CreateJobSkillAdd(player, jobId, skillId, 0));

        Get<EventBubblingSystem>().Publish(EventBubblingPlayerSkillAdd{
            .player = &player,
            .skill = skillComponent.FindSkill(skillData->index),
            });

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

            _serviceLocator.Get<GameRepositoryService>().SaveJobLevel(player, static_cast<int32_t>(job.GetId()), job.GetLevel(), job.GetSkillPoint(),
                std::ranges::to<std::vector>(skills | std::views::transform([cid = player.GetCId(), &job](int32_t skillId) -> req::SkillCreate
                    {
                        return req::SkillCreate{
                            .cid = cid,
                            .id = skillId,
                            .job = static_cast<int32_t>(job.GetId()),
                            .level = 1,
                        };
                    })));
            Get<PlayerStatSystem>().UpdateJobStat(player);

            Get<EntityViewRangeSystem>().Broadcast(player, GamePlayerMessageCreator::CreateJobExpLevelUp(player), true);
        }
        else
        {
            _serviceLocator.Get<GameRepositoryService>().SaveJobExp(player, static_cast<int32_t>(job.GetId()), job.GetExp());
        }

        player.FlushDeferred();
    }

    void PlayerJobSystem::OnLocalActivate(GamePlayer& player)
    {
        _serviceLocator.Get<ZoneExecutionService>().AddTimer(std::chrono::milliseconds(1200), player.GetCId(), _stageId,
            [](GamePlayer& player)
            {
                player.Defer(ItemArchiveMessageCreator::CreateItemAddForRefresh(player, GameConstant::QUICK_SLOT_DUMMY_ITEM_ID));
                player.Defer(ItemArchiveMessageCreator::CreateItemRemoveForRefresh(player));
                player.FlushDeferred();
            });
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

            _serviceLocator.Get<GameRepositoryService>().SaveSkillLevel(player,
                static_cast<int32_t>(job->GetId()), job->GetSkillPoint(), skill->GetId(), newLevel);

            player.Send(GamePlayerMessageCreator::CreateJobSkillPointChange(player, job->GetId(), job->GetSkillPoint(), false));

            Get<EventBubblingSystem>().Publish(EventBubblingPlayerSkillLevelChange{
                .player = &player,
                .skill = skill,
                .oldLevel = skillLevel,
                .newLevel = newLevel,
                });

            return;
            
        } while (false);

        SUNLIGHT_LOG_INFO(_serviceLocator,
            fmt::format("[{}] fail to handle. player: {}, error: {}",
                GetName(), player.GetCId(), error));
    }

    void PlayerJobSystem::OnSkillQuickSlotPositionSet(const ZoneMessage& message)
    {
        SlPacketReader& reader = message.reader;
        GamePlayer& player = message.player;

        const int32_t skillId = reader.Read<int32_t>();
        const int8_t page = static_cast<int8_t>(reader.Read<int32_t>());
        const int8_t x = static_cast<int8_t>(reader.Read<int32_t>());
        const int8_t y = static_cast<int8_t>(reader.Read<int32_t>());

        PlayerSkill* skill = player.GetSkillComponent().FindSkill(skillId);
        if (!skill)
        {
            return;
        }

        if (skill->GetPage() == page && skill->GetX() == x && skill->GetY() == y)
        {
            return;
        }

        skill->SetPage(page);
        skill->SetX(x);
        skill->SetX(y);

        _serviceLocator.Get<GameRepositoryService>().SaveSkillPosition(player, skillId, page, x, y);
    }

    auto PlayerJobSystem::GetJobGainSkills(JobId id, int32_t level) const -> std::vector<int32_t>
    {
        std::vector<int32_t> result;

        const SkillDataProvider& skillDataProvider = _serviceLocator.Get<GameDataProvideService>().GetSkillDataProvider();

        for (const PlayerSkillData& skillData : skillDataProvider.FindByJob(static_cast<int32_t>(id), level) | notnull::reference)
        {
            result.push_back(skillData.index);
        }

        if (!IsNovice(id))
        {
            const int32_t jobCommon = static_cast<int32_t>(id) % 1000 / 100 * 100;

            for (const PlayerSkillData& skillData : skillDataProvider.FindByJob(jobCommon, level) | notnull::reference)
            {
                result.push_back(skillData.index);
            }
        }

        return result;
    }

    bool PlayerJobSystem::IsNovice(JobId job)
    {
        switch (job)
        {
        case JobId::NoviceFighter:
        case JobId::NoviceRanger:
        case JobId::NoviceMagician:
        case JobId::NoviceArtisan:
            return true;
        }

        return false;
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
