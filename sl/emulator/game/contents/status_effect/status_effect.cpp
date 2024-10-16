#include "status_effect.h"

namespace sunlight
{
    StatusEffect::StatusEffect(int32_t skillId, StatusEffectType type, int32_t effectId, game_time_point_type endTimePoint, const SkillEffectData& skillEffectData)
        : _skillId(skillId)
        , _effectType(type)
        , _effectId(effectId)
        , _endTimePoint(endTimePoint)
        , _skillEffectData(&skillEffectData)
    {
    }

    bool StatusEffect::IsHidden() const
    {
        return _hidden;
    }

    auto StatusEffect::GetSkillId() const -> int32_t
    {
        return _skillId;
    }

    auto StatusEffect::GetType() const -> StatusEffectType
    {
        return _effectType;
    }

    auto StatusEffect::GetId() const -> int32_t
    {
        return _effectId;
    }

    auto StatusEffect::GetEndTimePoint() const -> game_time_point_type
    {
        return _endTimePoint;
    }

    auto StatusEffect::GetSkillEffectData() const -> const SkillEffectData&
    {
        assert(_skillEffectData);

        return *_skillEffectData;
    }

    void StatusEffect::SetHidden(bool value)
    {
        _hidden = value;
    }
}
