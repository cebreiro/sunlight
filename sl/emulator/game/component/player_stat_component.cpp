#include "player_stat_component.h"

#include "sl/emulator/game/data/sox/job_reference.h"
#include "sl/emulator/service/database/dto/character.h"

namespace sunlight
{
    PlayerStatComponent::PlayerStatComponent(const db::dto::Character& dto)
        : _dead(dto.hp.has_value() && *dto.hp == 0)
        , _gender(dto.gender)
        , _level(dto.level)
        , _exp(dto.exp)
        , _statPoint(dto.statPoint)
    {
        Mutable(RecoveryStatType::HP).SetValue(dto.hp.value_or(0));
        Mutable(RecoveryStatType::SP).SetValue(dto.sp.value_or(0));

        Mutable(PlayerStatType::Str).Set(StatOriginType::Base, dto.str);
        Mutable(PlayerStatType::Str).SetChanges(true);

        Mutable(PlayerStatType::Dex).Set(StatOriginType::Base, dto.dex);
        Mutable(PlayerStatType::Dex).SetChanges(true);

        Mutable(PlayerStatType::Accr).Set(StatOriginType::Base, dto.accr);
        Mutable(PlayerStatType::Accr).SetChanges(true);

        Mutable(PlayerStatType::Health).Set(StatOriginType::Base, dto.health);
        Mutable(PlayerStatType::Health).SetChanges(true);

        Mutable(PlayerStatType::Intell).Set(StatOriginType::Base, dto.intell);
        Mutable(PlayerStatType::Intell).SetChanges(true);

        Mutable(PlayerStatType::Wisdom).Set(StatOriginType::Base, dto.wis);
        Mutable(PlayerStatType::Wisdom).SetChanges(true);

        Mutable(PlayerStatType::Will).Set(StatOriginType::Base, dto.will);
        Mutable(PlayerStatType::Will).SetChanges(true);

        Mutable(PlayerStatType::Water).Set(StatOriginType::Base, dto.water);
        Mutable(PlayerStatType::Water).SetChanges(true);

        Mutable(PlayerStatType::Fire).Set(StatOriginType::Base, dto.fire);
        Mutable(PlayerStatType::Fire).SetChanges(true);

        Mutable(PlayerStatType::Lightning).Set(StatOriginType::Base, dto.lightning);
        Mutable(PlayerStatType::Lightning).SetChanges(true);

        constexpr int32_t regenTickOneSecond = 1000;

        Mutable(RecoveryStatType::HP).SetRegenTickCount(regenTickOneSecond);
        Mutable(RecoveryStatType::SP).SetRegenTickCount(regenTickOneSecond);
    }

    bool PlayerStatComponent::IsDead() const
    {
        return _dead;
    }

    void PlayerStatComponent::Suspend(RecoveryStatType type)
    {
        Mutable(type).SetDisable(true);
    }

    void PlayerStatComponent::Resume(RecoveryStatType type)
    {
        RecoveryStat& stat = Mutable(type);

        stat.SetDisable(false);
        stat.SetUpdateTimePoint(GameTimeService::Now());
    }

    void PlayerStatComponent::OnChangeMaxHP()
    {
        Mutable(RecoveryStatType::HP).Update(GameTimeService::Now());

        Mutable(RecoveryStatType::HP).SetMaxValue(GetFinalStat(PlayerStatType::MaxHP));
    }

    void PlayerStatComponent::OnChangeMaxSP()
    {
        Mutable(RecoveryStatType::SP).Update(GameTimeService::Now());

        Mutable(RecoveryStatType::SP).SetMaxValue(GetFinalStat(PlayerStatType::MaxSP));
    }

    void PlayerStatComponent::OnChangeRecoveryHP()
    {
        RecoveryStat& stat = Mutable(RecoveryStatType::HP);
        stat.Update(GameTimeService::Now());

        // client 48C620h
        const StatValue regen = GetFinalStat(PlayerStatType::RecoveryRateHP) * 0.2;

        stat.SetRegenValue(regen);
    }

    void PlayerStatComponent::OnChangeRecoverySP()
    {
        RecoveryStat& stat = Mutable(RecoveryStatType::SP);
        stat.Update(GameTimeService::Now());

        // client 48C620h
        const StatValue regen = GetFinalStat(PlayerStatType::RecoveryRateSP) * 0.25;

        stat.SetRegenValue(regen);
    }

    auto PlayerStatComponent::GetFinalStat(RecoveryStatType type) const -> StatValue
    {
        return const_cast<PlayerStatComponent*>(this)->Mutable(type).GetValue(GameTimeService::Now());
    }

    auto PlayerStatComponent::GetFinalStat(PlayerStatType type) const -> StatValue
    {
        const Stat& stat = Get(type);

        if (stat.HasChanges())
        {
            const_cast<PlayerStatComponent*>(this)->UpdateFinalStat(type);

            assert(!stat.HasChanges());
        }

        return stat.GetFinalValue();
    }

    auto PlayerStatComponent::GetLevel() const -> int32_t
    {
        return _level;
    }

    auto PlayerStatComponent::GetExp() const -> int32_t
    {
        return _exp;
    }

    auto PlayerStatComponent::GetStatPoint() const -> int32_t
    {
        return _statPoint;
    }

    auto PlayerStatComponent::Get(PlayerStatType type) const -> const Stat&
    {
        static Stat nullObject;

        const auto iter = _stats.find(type);

        return iter != _stats.end() ? iter->second : nullObject;
    }

    void PlayerStatComponent::SetDead(bool value)
    {
        _dead = value;
    }

    void PlayerStatComponent::SetRecoveryTimePoint(RecoveryStatType type, game_time_point_type timePoint)
    {
        Mutable(type).SetUpdateTimePoint(timePoint);
    }

    void PlayerStatComponent::SetJobReferenceStat(PlayerStatType type, StatValue value)
    {
        Stat& stat = Mutable(type);

        stat.Set(StatOriginType::JobReference, value);
        stat.SetChanges(true);
    }

    void PlayerStatComponent::SetRecoveryStat(RecoveryStatType type, StatValue value)
    {
        Mutable(type).SetValue(value);
    }

    void PlayerStatComponent::SetLevel(int32_t level)
    {
        _level = level;
    }

    void PlayerStatComponent::SetExp(int32_t exp)
    {
        _exp = exp;
    }

    void PlayerStatComponent::SetStatPoint(int32_t point)
    {
        _statPoint = point;
    }

    void PlayerStatComponent::UpdateFinalStat(PlayerStatType type)
    {
        Stat& stat = Mutable(type);
        if (!stat.HasChanges())
        {
            return;
        }

        stat.SetChanges(false);

        stat.SetFinalValue(CalculateStatSum(type));
    }

    auto PlayerStatComponent::CalculateStatSum(PlayerStatType type) const -> StatValue
    {
        const Stat& stat = Get(type);

        StatValue allStat(0.0);

        switch (type)
        {
        case PlayerStatType::Str:
        case PlayerStatType::Dex:
        case PlayerStatType::Accr:
        case PlayerStatType::Health:
        case PlayerStatType::Intell:
        case PlayerStatType::Wisdom:
        case PlayerStatType::Will:
        {
            allStat = GetFinalStat(PlayerStatType::AllBaseStat);
        }
        break;
        case PlayerStatType::Fire:
        case PlayerStatType::Water:
        case PlayerStatType::Lightning:
        {
            allStat = GetFinalStat(PlayerStatType::AllElemental);
        }
        break;
        default:;
        }

        const StatValue base = stat.Get(StatOriginType::Base);

        const StatValue sum = base + stat.Get(StatOriginType::Item) + stat.Get(StatOriginType::Skill) + stat.Get(StatOriginType::StatusEffect);
        const StatValue percentage = base * stat.Get(StatOriginType::StatusEffectPercentage);

        const StatValue result = sum + percentage + allStat + stat.Get(StatOriginType::JobReference);

        return result;
    }

    auto PlayerStatComponent::GetGender() const -> int8_t
    {
        return _gender;
    }

    auto PlayerStatComponent::Mutable(RecoveryStatType type) -> RecoveryStat&
    {
        const int64_t index = static_cast<int64_t>(type);
        assert(index >= 0 && index < std::ssize(_recoveryStats));

        return _recoveryStats[index];
    }

    auto PlayerStatComponent::Mutable(PlayerStatType type) -> Stat&
    {
        return _stats[type];
    }
}
