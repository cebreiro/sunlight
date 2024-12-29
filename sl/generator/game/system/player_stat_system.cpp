#include "player_stat_system.h"

#include "sl/generator/game/game_constant.h"
#include "sl/generator/game/component/player_job_component.h"
#include "sl/generator/game/component/player_stat_component.h"
#include "sl/generator/game/data/sox/job_reference.h"
#include "sl/generator/game/entity/game_player.h"
#include "sl/generator/game/message/zone_message.h"
#include "sl/generator/game/message/creator/game_player_message_creator.h"
#include "sl/generator/game/message/creator/status_message_creator.h"
#include "sl/generator/game/system/entity_view_range_system.h"
#include "sl/generator/game/zone/stage.h"
#include "sl/generator/game/zone/service/game_repository_service.h"
#include "sl/generator/service/gamedata/gamedata_provide_service.h"
#include "sl/generator/service/gamedata/exp/character_exp_data.h"
#include "sl/generator/service/gamedata/exp/exp_data_provider.h"

namespace sunlight
{
    PlayerStatSystem::PlayerStatSystem(const ServiceLocator& serviceLocator)
        : _serviceLocator(serviceLocator)
    {
    }

    void PlayerStatSystem::InitializeSubSystem(Stage& stage)
    {
        Add(stage.Get<EntityViewRangeSystem>());
    }

    bool PlayerStatSystem::Subscribe(Stage& stage)
    {
        (void)stage;

        return true;
    }

    auto PlayerStatSystem::GetName() const -> std::string_view
    {
        return "player_stat_system";
    }

    auto PlayerStatSystem::GetClassId() const -> game_system_id_type
    {
        return GameSystem::GetClassId<PlayerStatSystem>();
    }

    void PlayerStatSystem::GainCharacterExp(GamePlayer& player, int32_t exp)
    {
        PlayerStatComponent& statComponent = player.GetStatComponent();
        const int32_t level = statComponent.GetLevel();

        if (level >= GameConstant::PLAYER_CHARACTER_LEVEL_MAX)
        {
            return;
        }

        const ExpDataProvider& expDataProvider = _serviceLocator.Get<GameDataProvideService>().GetExpDataProvider();
        const CharacterExpData* data = expDataProvider.FindCharacterData(level + 1);

        if (!data)
        {
            return;
        }

        statComponent.SetExp(statComponent.GetExp() + exp);
        player.Send(GamePlayerMessageCreator::CreateCharacterExpGain(player, exp));

        if (statComponent.GetExp() >= data->expMax)
        {
            statComponent.SetLevel(level + 1);
            statComponent.SetExp(0);
            statComponent.SetStatPoint(statComponent.GetStatPoint() + GameConstant::STAT_POINT_PER_CHARACTER_LEVEL_UP);

            _serviceLocator.Get<GameRepositoryService>().SaveCharacterLevel(player, statComponent.GetLevel(), statComponent.GetStatPoint());

            Get<EntityViewRangeSystem>().Broadcast(player,
                GamePlayerMessageCreator::CreateCharacterLevelUp(player), true);
        }
        else
        {
            _serviceLocator.Get<GameRepositoryService>().SaveCharacterExp(player, statComponent.GetExp());
        }
    }

    void PlayerStatSystem::RecoverHP(GamePlayer& player, HPChangeFloaterType floater)
    {
        PlayerStatComponent& statComponent = player.GetStatComponent();

        const int32_t maxHP = statComponent.GetFinalStat(PlayerStatType::MaxHP).As<int32_t>();
        statComponent.SetRecoveryStat(RecoveryStatType::HP, maxHP);

        Get<EntityViewRangeSystem>().Broadcast(player,
            StatusMessageCreator::CreateHPChange(player, maxHP, maxHP, floater), true);
    }

    void PlayerStatSystem::RecoverSP(GamePlayer& player, SPChangeFloaterType floater)
    {
        PlayerStatComponent& statComponent = player.GetStatComponent();

        const int32_t maxSP = statComponent.GetFinalStat(PlayerStatType::MaxSP).As<int32_t>();
        statComponent.SetRecoveryStat(RecoveryStatType::SP, maxSP);

        Get<EntityViewRangeSystem>().Broadcast(player,
            StatusMessageCreator::CreateSPChange(player, maxSP, maxSP, floater), true);
    }

    void PlayerStatSystem::SetHP(GamePlayer& player, int32_t value, HPChangeFloaterType floater)
    {
        PlayerStatComponent& statComponent = player.GetStatComponent();

        const int32_t maxHP = statComponent.GetFinalStat(PlayerStatType::MaxHP).As<int32_t>();
        const int32_t hp = std::clamp(value, 0, maxHP);

        statComponent.SetRecoveryStat(RecoveryStatType::HP, StatValue(hp));
        statComponent.SetRecoveryTimePoint(RecoveryStatType::HP, GameTimeService::Now());

        Get<EntityViewRangeSystem>().Broadcast(player,
            StatusMessageCreator::CreateHPChange(player, maxHP, hp, floater), true);
    }

    void PlayerStatSystem::SetSP(GamePlayer& player, int32_t value, SPChangeFloaterType floater)
    {
        PlayerStatComponent& statComponent = player.GetStatComponent();

        const int32_t maxSP = statComponent.GetFinalStat(PlayerStatType::MaxSP).As<int32_t>();
        const int32_t sp = std::clamp(value, 0, maxSP);

        statComponent.SetRecoveryStat(RecoveryStatType::SP, StatValue(sp));
        statComponent.SetRecoveryTimePoint(RecoveryStatType::SP, GameTimeService::Now());

        Get<EntityViewRangeSystem>().Broadcast(player,
            StatusMessageCreator::CreateSPChange(player, maxSP, sp, floater), true);
    }

    void PlayerStatSystem::ApplyDamage(GamePlayer& player, int32_t damage, int32_t& outMaxHP, int32_t& outNewHP)
    {
        PlayerStatComponent& statComponent = player.GetStatComponent();

        const int32_t maxHP = statComponent.GetFinalStat(PlayerStatType::MaxHP).As<int32_t>();
        const int32_t current = statComponent.GetFinalStat(RecoveryStatType::HP).As<int32_t>();

        const int32_t newHP = std::max(0, current - damage);
        statComponent.SetRecoveryStat(RecoveryStatType::HP, newHP);

        if (newHP <= 0)
        {
            player.SetDead(true);
            statComponent.Suspend(RecoveryStatType::HP);
            statComponent.Suspend(RecoveryStatType::SP);
        }

        outMaxHP = maxHP;
        outNewHP = newHP;
    }

    void PlayerStatSystem::UpdateJobStat(GamePlayer& player)
    {
        PlayerStatComponent& statComponent = player.GetComponent<PlayerStatComponent>();
        PlayerJobComponent& jobComponent = player.GetComponent<PlayerJobComponent>();

        const Job* novice = jobComponent.Find(JobType::Novice);
        const Job* advanced = jobComponent.Find(JobType::Advanced);

        const int32_t levelSum = (novice ? novice->GetLevel() : 0) + (advanced ? advanced->GetLevel() - 1 : 0);
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

        const StatValue jobRecoverySP = CalculateJobRecoverySP(*data, statComponent);
        statComponent.SetJobReferenceStat(PlayerStatType::RecoveryRateSP, jobRecoverySP);

        statComponent.OnChangeRecoveryHP();
        statComponent.OnChangeRecoverySP();
    }

    void PlayerStatSystem::UpdateRegenStat(GamePlayer& player)
    {
        PlayerStatComponent& statComponent = player.GetComponent<PlayerStatComponent>();

        if (statComponent.IsStatChanged(PlayerStatType::RecoveryRateHP))
        {
            statComponent.OnChangeRecoveryHP();
        }

        if (statComponent.IsStatChanged(PlayerStatType::RecoveryRateSP))
        {
            statComponent.OnChangeRecoverySP();
        }
    }

    void PlayerStatSystem::OnInitialize(GamePlayer& player)
    {
        UpdateJobStat(player);

        PlayerStatComponent& statComponent = player.GetComponent<PlayerStatComponent>();

        statComponent.SetRecoveryStateFactor(RecoveryStatType::HP, StatValue(0.2));
        statComponent.SetRecoveryStateFactor(RecoveryStatType::SP, StatValue(0.25));
        UpdateRegenStat(player);

        if (!player.IsDead())
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

    void PlayerStatSystem::OnLocalActivate(GamePlayer& player)
    {
        PlayerStatComponent& statComponent = player.GetComponent<PlayerStatComponent>();

        statComponent.Resume(RecoveryStatType::HP);
        statComponent.Resume(RecoveryStatType::SP);
    }

    void PlayerStatSystem::OnStateChange(GamePlayer& player, GameEntityStateType oldState, GameEntityStateType newState)
    {
        if (oldState == GameEntityStateType::Sitting && newState != GameEntityStateType::Sitting)
        {
            PlayerStatComponent& statComponent = player.GetStatComponent();

            statComponent.SetRecoveryStateFactor(RecoveryStatType::HP, StatValue(0.2));
            statComponent.SetRecoveryStateFactor(RecoveryStatType::SP, StatValue(0.25));

            statComponent.OnChangeRecoveryHP();
            statComponent.OnChangeRecoverySP();
        }
        else if (oldState != GameEntityStateType::Sitting && newState == GameEntityStateType::Sitting)
        {
            PlayerStatComponent& statComponent = player.GetStatComponent();

            statComponent.SetRecoveryStateFactor(RecoveryStatType::HP, StatValue(1.0));
            statComponent.SetRecoveryStateFactor(RecoveryStatType::SP, StatValue(1.0));

            statComponent.OnChangeRecoveryHP();
            statComponent.OnChangeRecoverySP();
        }
    }

    void PlayerStatSystem::OnItemEquip(GamePlayer& player, const GameItem& item)
    {
        (void)player;
        (void)item;
    }

    void PlayerStatSystem::OnItemUnequip(GamePlayer& player, const GameItem& item)
    {
        (void)player;
        (void)item;
    }

    void PlayerStatSystem::OnItemEquipmentChange(GamePlayer& player, const GameItem& removed, const GameItem& added)
    {
        (void)player;
        (void)removed;
        (void)added;
    }

    void PlayerStatSystem::OnStatPointUse(const ZoneMessage& message)
    {
        GamePlayer& player = message.player;
        PlayerStatComponent& statComponent = player.GetStatComponent();

        SlPacketReader& reader = message.reader;

        std::array<int32_t, 7> addStats = {};
        addStats[0] = reader.Read<int32_t>();
        addStats[1] = reader.Read<int32_t>();
        addStats[2] = reader.Read<int32_t>();
        addStats[3] = reader.Read<int32_t>();
        addStats[4] = reader.Read<int32_t>();
        addStats[5] = reader.Read<int32_t>();
        addStats[6] = reader.Read<int32_t>();

        const bool hasNegativeValue = std::ranges::any_of(addStats, [](int32_t addValue)
            {
                return addValue < 0;
            });
        const int32_t sum = std::accumulate(addStats.begin(), addStats.end(), int32_t{ 0 },
            [](int32_t sum, int32_t value) -> int32_t
            {
                return sum + value;
            });

        if (hasNegativeValue || sum > statComponent.GetStatPoint())
        {
            SUNLIGHT_LOG_WARN(_serviceLocator,
                fmt::format("[{}] player cheat request. cid: {}, stats: {{ {}, {}, {}, {}, {}, {}, {} }}, sum: {}, stat_point: {}",
                    GetName(), player.GetCId(),
                    addStats[0], addStats[1], addStats[2], addStats[3], addStats[4], addStats[5], addStats[6],
                    sum, statComponent.GetStatPoint()));

            return;
        }

        statComponent.SetStatPoint(statComponent.GetStatPoint() - sum);
        statComponent.AddStat(PlayerStatType::Str, StatOriginType::Base, addStats[0]);
        statComponent.AddStat(PlayerStatType::Dex, StatOriginType::Base, addStats[1]);
        statComponent.AddStat(PlayerStatType::Accr, StatOriginType::Base, addStats[2]);
        statComponent.AddStat(PlayerStatType::Health, StatOriginType::Base, addStats[3]);
        statComponent.AddStat(PlayerStatType::Intell, StatOriginType::Base, addStats[4]);
        statComponent.AddStat(PlayerStatType::Wisdom, StatOriginType::Base, addStats[5]);
        statComponent.AddStat(PlayerStatType::Will, StatOriginType::Base, addStats[6]);

        _serviceLocator.Get<GameRepositoryService>().SaveStat(player,
            statComponent.GetStatPoint(),
            statComponent.Get(PlayerStatType::Str).Get(StatOriginType::Base).As<int32_t>(),
            statComponent.Get(PlayerStatType::Dex).Get(StatOriginType::Base).As<int32_t>(),
            statComponent.Get(PlayerStatType::Accr).Get(StatOriginType::Base).As<int32_t>(),
            statComponent.Get(PlayerStatType::Health).Get(StatOriginType::Base).As<int32_t>(),
            statComponent.Get(PlayerStatType::Intell).Get(StatOriginType::Base).As<int32_t>(),
            statComponent.Get(PlayerStatType::Wisdom).Get(StatOriginType::Base).As<int32_t>(),
            statComponent.Get(PlayerStatType::Will).Get(StatOriginType::Base).As<int32_t>());
    }

    auto PlayerStatSystem::CalculateJobMaxHP(const sox::JobReference& data, int32_t jobLevel,
        const PlayerStatComponent& statComponent) -> StatValue
    {
        const double health = statComponent.GetFinalStat(PlayerStatType::Health).As<double>();

        return (4.0 * (health + 2.0 * (jobLevel)) * data.mHPFactor) + 10.0;
    }

    auto PlayerStatSystem::CalculateJobMaxSP(const sox::JobReference& data, int32_t jobLevel,
        const PlayerStatComponent& statComponent) -> StatValue
    {
        const double intell = statComponent.GetFinalStat(PlayerStatType::Intell).As<double>();
        const double wisdom = statComponent.GetFinalStat(PlayerStatType::Wisdom).As<double>();
        const double will = statComponent.GetFinalStat(PlayerStatType::Will).As<double>();

        return (2.0 * (intell + wisdom + will + 5.0 * (jobLevel))) * data.mSPFactor;
    }

    auto PlayerStatSystem::CalculateJobRecoveryHP(const sox::JobReference& data,
        const PlayerStatComponent& statComponent) -> StatValue
    {
        const double health = statComponent.GetFinalStat(PlayerStatType::Health).As<double>();

        return data.hPRecoveryFactor * health * 0.2 + 2.0;
    }

    auto PlayerStatSystem::CalculateJobRecoverySP(const sox::JobReference& data,
        const PlayerStatComponent& statComponent) -> StatValue
    {
        const double will = statComponent.GetFinalStat(PlayerStatType::Will).As<double>();

        return data.sPRecoveryFactor * will * 0.15 + 2.0;
    }
}
