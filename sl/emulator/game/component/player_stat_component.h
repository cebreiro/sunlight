#pragma once
#include "sl/emulator/game/component/game_component.h"
#include "sl/emulator/game/contants/stat/player_stat_type.h"
#include "sl/emulator/game/contants/stat/stat.h"
#include "sl/emulator/game/contants/stat/recovery_stat.h"
#include "sl/emulator/game/time/game_time_service.h"

namespace sunlight::db::dto
{
    struct Character;
}

namespace sunlight
{
    class PlayerStatComponent final : public GameComponent
    {
    public:
        explicit PlayerStatComponent(const db::dto::Character& dto);

        bool IsDead() const;

        auto GetGender() const -> int8_t;
        auto GetLevel() const -> int32_t;
        auto GetExp() const -> int32_t;
        auto GetStatPoint() const -> int32_t;

        auto GetStatValue(RecoveryStatType type, game_time_point_type current = GameTimeService::Get()) const -> StatValue;

        auto GetStat(PlayerStatType type) const -> const Stat&;

        void SetDead(bool value);
        void SetRegenValue(RecoveryStatType type, float value);

    private:
        auto Mutable(RecoveryStatType type) -> RecoveryStat&;
        auto Mutable(PlayerStatType type) -> Stat&;

    private:
        bool _dead = false;
        int8_t _gender = 0;
        int32_t _level = 0;
        int32_t _exp = 0;
        int32_t _statPoint = 0;

        std::array<RecoveryStat, static_cast<int32_t>(RecoveryStatType::Count)> _recoveryStats = {};
        std::unordered_map<PlayerStatType, Stat> _stats;
    };
}
