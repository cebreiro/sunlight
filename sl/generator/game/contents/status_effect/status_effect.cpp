#include "status_effect.h"

#include "sl/generator/service/gamedata/skill/skill_effect_data.h"


namespace sunlight
{
    StatusEffect::StatusEffect(int32_t skillId, int32_t skillLevel, const SkillEffectData& skillEffectData,
        game_time_point_type now, game_time_point_type endTimePoint)
        : _skillId(skillId)
        , _skillLevel(skillLevel)
        , _type(static_cast<StatusEffectType>(skillEffectData.type))
        , _id(GetStatusEffectIdFrom(skillEffectData))
        , _endTimePoint(endTimePoint)
        , _lastTickTimePoint(now)
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

    auto StatusEffect::GetSkillLevel() const -> int32_t
    {
        return _skillLevel;
    }

    auto StatusEffect::GetType() const -> StatusEffectType
    {
        return _type;
    }

    auto StatusEffect::GetId() const -> int32_t
    {
        return _id;
    }

    auto StatusEffect::GetEndTimePoint() const -> game_time_point_type
    {
        return _endTimePoint;
    }

    auto StatusEffect::GetLastTickTimePoint() const -> game_time_point_type
    {
        return _lastTickTimePoint;
    }

    auto StatusEffect::GetNextTickTimePoint() const -> game_time_point_type
    {
        return _nextTickTimePoint;
    }

    auto StatusEffect::GetData() const -> const SkillEffectData&
    {
        assert(_skillEffectData);

        return *_skillEffectData;
    }

    auto StatusEffect::GetStatType() const -> int32_t
    {
        if (_type == StatusEffectType::StatIncrease || _type == StatusEffectType::Heal)
        {
            return GetData().value7;
        }

        return 0;
    }

    auto StatusEffect::GetStatValue() const -> int32_t
    {
        if (_type == StatusEffectType::StatIncrease || _type == StatusEffectType::Heal)
        {
            const SkillEffectData& data = GetData();

            return data.value2 + data.value1 * GetSkillLevel() + data.value8;
        }

        return 0;
    }

    auto StatusEffect::GetStatPercentageValue() const -> int32_t
    {
        if (_type == StatusEffectType::StatIncrease || _type == StatusEffectType::Heal)
        {
            return GetData().value9;
        }

        return 0;
    }

    void StatusEffect::SetHidden(bool value)
    {
        _hidden = value;
    }

    void StatusEffect::SetLastTickTimePoint(game_time_point_type timePoint)
    {
        _lastTickTimePoint = timePoint;
    }

    void StatusEffect::SetNextTickTimePoint(game_time_point_type timePoint)
    {
        _nextTickTimePoint = timePoint;
    }

    auto StatusEffect::GetStatusEffectIdFrom(const SkillEffectData& data) -> int32_t
    {
        return data.value5;
    }
}
