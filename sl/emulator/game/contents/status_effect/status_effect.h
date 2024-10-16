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
        StatusEffect(int32_t skillId, StatusEffectType type, int32_t effectId, game_time_point_type endTimePoint,
            const SkillEffectData& skillEffectData);

        bool IsHidden() const;

        auto GetSkillId() const -> int32_t;
        auto GetType() const -> StatusEffectType;
        auto GetId() const -> int32_t;
        auto GetEndTimePoint() const -> game_time_point_type;
        auto GetSkillEffectData() const -> const SkillEffectData&;

        void SetHidden(bool value);

    private:
        bool _hidden = false;

        int32_t _skillId = 0;
        StatusEffectType _effectType = {};
        int32_t _effectId = 0;
        game_time_point_type _endTimePoint = {};

        PtrNotNull<const SkillEffectData> _skillEffectData = nullptr;
    };
}
