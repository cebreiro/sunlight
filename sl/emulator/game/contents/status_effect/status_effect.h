#pragma once
#include "sl/emulator/game/time/game_time.h"
#include "sl/emulator/service/gamedata/skill/status_effect_type.h"

namespace sunlight
{
    struct SkillEffectData;
}

namespace sunlight
{
    class StatusEffect
    {
    public:
        StatusEffect() = default;
        StatusEffect(int32_t skillId, int32_t skillLevel, const SkillEffectData& skillEffectData,
            game_time_point_type now, std::chrono::milliseconds duration);

        bool IsHidden() const;

        auto GetSkillId() const -> int32_t;
        auto GetSkillLevel() const -> int32_t;
        auto GetType() const -> StatusEffectType;
        auto GetId() const -> int32_t;
        auto GetEndTimePoint() const -> game_time_point_type;
        auto GetLastTickTimePoint() const -> game_time_point_type;
        auto GetNextTickTimePoint() const -> game_time_point_type;
        auto GetData() const -> const SkillEffectData&;

        auto GetStatType() const -> int32_t;
        auto GetStatValue() const -> int32_t;
        auto GetStatPercentageValue() const -> int32_t;

        void SetHidden(bool value);
        void SetLastTickTimePoint(game_time_point_type timePoint);
        void SetNextTickTimePoint(game_time_point_type timePoint);

    private:
        bool _hidden = false;

        int32_t _skillId = 0;
        int32_t _skillLevel = 0;
        StatusEffectType _type = {};
        int32_t _id = 0;
        game_time_point_type _endTimePoint = {};
        game_time_point_type _lastTickTimePoint = {};
        game_time_point_type _nextTickTimePoint = {};

        PtrNotNull<const SkillEffectData> _skillEffectData = nullptr;
    };
}
