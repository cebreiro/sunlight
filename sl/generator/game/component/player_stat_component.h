#pragma once
#include "sl/generator/game/component/game_component.h"
#include "sl/generator/game/contents/stat/player_stat_type.h"
#include "sl/generator/game/contents/stat/stat.h"
#include "sl/generator/game/contents/stat/recovery_stat.h"
#include "sl/generator/game/time/game_time_service.h"

namespace sunlight::db::dto
{
    struct Character;
}

namespace sunlight::sox
{
    struct JobReference;
}

namespace sunlight
{
    class PlayerStatComponent final : public GameComponent
    {
    public:
        explicit PlayerStatComponent(const db::dto::Character& dto);

        bool IsStatChanged(PlayerStatType type) const;

        void Suspend(RecoveryStatType type);
        void Resume(RecoveryStatType type);

        void AddStat(PlayerStatType type, StatOriginType origin, StatValue value);

        void OnChangeMaxHP();
        void OnChangeMaxSP();
        void OnChangeRecoveryHP();
        void OnChangeRecoverySP();

        auto GetGender() const -> int8_t;
        auto GetLevel() const -> int32_t;
        auto GetExp() const -> int32_t;
        auto GetStatPoint() const -> int32_t;

        auto GetFinalStat(RecoveryStatType type) const -> StatValue;
        auto GetFinalStat(PlayerStatType type) const -> StatValue;

        auto Get(PlayerStatType type) const -> const Stat&;

        void SetRecoveryTimePoint(RecoveryStatType type, game_time_point_type timePoint);
        void SetJobReferenceStat(PlayerStatType type, StatValue value);
        void SetRecoveryStat(RecoveryStatType type, StatValue value);
        void SetRecoveryStateFactor(RecoveryStatType type, StatValue value);

        void SetLevel(int32_t level);
        void SetExp(int32_t exp);
        void SetStatPoint(int32_t point);

    private:
        void UpdateFinalStat(PlayerStatType type);

        auto CalculateStat(PlayerStatType type, bool includePassive = true, bool includeStatusEffect = true) const -> StatValue;

        auto Mutable(RecoveryStatType type) -> RecoveryStat&;
        auto Mutable(PlayerStatType type) -> Stat&;

    private:
        int8_t _gender = 0;
        int32_t _level = 0;
        int32_t _exp = 0;
        int32_t _statPoint = 0;

        std::array<RecoveryStat, static_cast<int32_t>(RecoveryStatType::Count)> _recoveryStats = {};
        std::unordered_map<PlayerStatType, Stat> _stats;
    };
}
