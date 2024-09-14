#include "player_stat_update_system.h"

#include "sl/emulator/game/component/player_item_component.h"
#include "sl/emulator/game/component/player_job_component.h"
#include "sl/emulator/game/component/player_stat_component.h"
#include "sl/emulator/game/data/sox/job_reference.h"
#include "sl/emulator/game/entity/game_item.h"
#include "sl/emulator/game/entity/game_player.h"

namespace sunlight
{
    PlayerStatUpdateSystem::PlayerStatUpdateSystem(const ServiceLocator& serviceLocator)
        : _serviceLocator(serviceLocator)
    {
    }

    auto PlayerStatUpdateSystem::GetName() const -> std::string_view
    {
        return "player_recovery_system";
    }

    auto PlayerStatUpdateSystem::GetClassId() const -> game_system_id_type
    {
        return GameSystem::GetClassId<PlayerStatUpdateSystem>();
    }

    void PlayerStatUpdateSystem::AddItemStat(GamePlayer& player, const GameItem& item)
    {
        (void)player;
        (void)item;
    }

    void PlayerStatUpdateSystem::RemoveItemStat(GamePlayer& player, const GameItem& item)
    {
        (void)player;
        (void)item;
    }

    void PlayerStatUpdateSystem::OnInitialize(GamePlayer& player)
    {
        PlayerStatComponent& statComponent = player.GetComponent<PlayerStatComponent>();
        PlayerJobComponent& jobComponent = player.GetComponent<PlayerJobComponent>();

        const Job* novice = jobComponent.GetIf(JobType::Novice);
        const Job* advanced = jobComponent.GetIf(JobType::Advanced);

        const int32_t levelSum = (novice ? novice->GetLevel() : 0) + (advanced ? advanced->GetLevel() : 0);
        const sox::JobReference* data = advanced ? &advanced->GetData() : (novice ? &novice->GetData() : nullptr);

        if (!data)
        {
            SUNLIGHT_LOG_ERROR(_serviceLocator,
                fmt::format("[{}] player has no job. player_cid: {}",
                    GetName(), player.GetCId()));

            return;
        }

        const StatValue jobMaxHP = CalculateJobMaxHP(*data, levelSum, statComponent);
        statComponent.SetJobReferenceStat(PlayerStatType::MaxHP, jobMaxHP);
        statComponent.OnChangeMaxHP();

        const StatValue jobMaxSP = CalculateJobMaxSP(*data, levelSum, statComponent);
        statComponent.SetJobReferenceStat(PlayerStatType::MaxSP, jobMaxSP);
        statComponent.OnChangeMaxSP();

        const StatValue jobRecoveryHP = CalculateJobRecoveryHP(*data, statComponent);
        statComponent.SetJobReferenceStat(PlayerStatType::RecoveryRateHP, jobRecoveryHP);
        statComponent.OnChangeRecoveryHP();

        const StatValue jobRecoverySP = CalculateJobRecoverySP(*data, statComponent);
        statComponent.SetJobReferenceStat(PlayerStatType::RecoveryRateSP, jobRecoverySP);
        statComponent.OnChangeRecoverySP();

        if (!statComponent.IsDead())
        {
            if (const double hp = statComponent.GetFinalStat(RecoveryStatType::HP).As<double>();
                hp <= 0.0)
            {
                statComponent.SetRecoveryStat(RecoveryStatType::HP,

                    statComponent.GetFinalStat(PlayerStatType::MaxHP));
            }

            if (const double sp = statComponent.GetFinalStat(RecoveryStatType::SP).As<double>();
                sp <= 0.0)
            {
                statComponent.SetRecoveryStat(RecoveryStatType::SP,
                    statComponent.GetFinalStat(PlayerStatType::MaxSP));
            }
        }
    }

    void PlayerStatUpdateSystem::OnLocalActivate(GamePlayer& player)
    {
        PlayerStatComponent& statComponent = player.GetComponent<PlayerStatComponent>();

        statComponent.Resume(RecoveryStatType::HP);
        statComponent.Resume(RecoveryStatType::SP);
    }

    auto PlayerStatUpdateSystem::CalculateJobMaxHP(const sox::JobReference& data, int32_t jobLevel,
        const PlayerStatComponent& statComponent) -> StatValue
    {
        const double health = statComponent.GetFinalStat(PlayerStatType::Health).As<double>();

        // client 487500h
        return (4.0 * (health + 2.0 * (jobLevel)) * data.mHPFactor) + 10.0;
    }

    auto PlayerStatUpdateSystem::CalculateJobMaxSP(const sox::JobReference& data, int32_t jobLevel,
        const PlayerStatComponent& statComponent) -> StatValue
    {
        const double intell = statComponent.GetFinalStat(PlayerStatType::Intell).As<double>();
        const double wisdom = statComponent.GetFinalStat(PlayerStatType::Wisdom).As<double>();
        const double will = statComponent.GetFinalStat(PlayerStatType::Will).As<double>();

        // client 487610h
        return (2.0 * (intell + wisdom + will + 5.0 * (jobLevel))) * data.mSPFactor;
    }

    auto PlayerStatUpdateSystem::CalculateJobRecoveryHP(const sox::JobReference& data,
        const PlayerStatComponent& statComponent) -> StatValue
    {
        const double health = statComponent.GetFinalStat(PlayerStatType::Health).As<double>();

        // client 4875C0h
        return data.hPRecoveryFactor * health * 0.2 + 2.0;
    }

    auto PlayerStatUpdateSystem::CalculateJobRecoverySP(const sox::JobReference& data,
        const PlayerStatComponent& statComponent) -> StatValue
    {
        const double will = statComponent.GetFinalStat(PlayerStatType::Will).As<double>();

        // client 487700h
        return data.sPRecoveryFactor * will * 0.15 + 2.0;
    }
}
