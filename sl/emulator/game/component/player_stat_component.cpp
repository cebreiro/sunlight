#include "player_stat_component.h"

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
        Mutable(PlayerStatType::Dex).Set(StatOriginType::Base, dto.dex);
        Mutable(PlayerStatType::Accr).Set(StatOriginType::Base, dto.accr);
        Mutable(PlayerStatType::Health).Set(StatOriginType::Base, dto.health);
        Mutable(PlayerStatType::Intell).Set(StatOriginType::Base, dto.intell);
        Mutable(PlayerStatType::Wisdom).Set(StatOriginType::Base, dto.wis);
        Mutable(PlayerStatType::Will).Set(StatOriginType::Base, dto.will);
        Mutable(PlayerStatType::Water).Set(StatOriginType::Base, dto.water);
        Mutable(PlayerStatType::Fire).Set(StatOriginType::Base, dto.fire);
        Mutable(PlayerStatType::Lightning).Set(StatOriginType::Base, dto.lightning);
    }

    bool PlayerStatComponent::IsDead() const
    {
        return _dead;
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

    auto PlayerStatComponent::GetStat(PlayerStatType type) const -> const Stat&
    {
        static Stat nullObject;

        const auto iter = _stats.find(type);

        return iter != _stats.end() ? iter->second : nullObject;
    }

    void PlayerStatComponent::SetDead(bool value)
    {
        _dead = value;
    }

    auto PlayerStatComponent::GetGender() const -> int8_t
    {
        return _gender;
    }

    auto PlayerStatComponent::GetStatValue(RecoveryStatType type, game_time_point_type current) const -> StatValue
    {
        RecoveryStat& stat = const_cast<PlayerStatComponent*>(this)->Mutable(type);

        return stat.GetValue(current);
    }

    void PlayerStatComponent::SetRegenValue(RecoveryStatType type, float value)
    {
        Mutable(type).SetRegenValue(value);
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
