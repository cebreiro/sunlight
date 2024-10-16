#include "status_effect.h"

#include "sl/emulator/service/gamedata/skill/player_skill_effect_data.h"

namespace sunlight
{
    StatusEffect::StatusEffect(int32_t skillId, int32_t skillLevel, StatusEffectType type,
        int32_t id, game_time_point_type endTimePoint, const SkillEffectStatusEffect& skillEffectData)
        : _skillId(skillId)
        , _skillLevel(skillLevel)
        , _type(type)
        , _id(id)
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

    auto StatusEffect::GetData() const -> const SkillEffectStatusEffect&
    {
        assert(_skillEffectData);

        return *_skillEffectData;
    }

    auto StatusEffect::GetStatType() const -> int32_t
    {
        if (_type == StatusEffectType::StatIncrease)
        {
            return GetData().GetRawData().value7;
        }

        return 0;
    }

    auto StatusEffect::GetStatValue() const -> int32_t
    {
        if (_type == StatusEffectType::StatIncrease)
        {
            const SkillEffectStatusEffect& data = GetData();

            return data.GetBaseValue() + data.GetValuePerSkillLevel() * GetSkillLevel() + data.GetRawData().value8;
        }

        return 0;
    }

    auto StatusEffect::GetStatPercentageValue() const -> int32_t
    {
        if (_type == StatusEffectType::StatIncrease)
        {
            const SkillEffectStatusEffect& data = GetData();

            return data.GetRawData().value9;
        }

        return 0;
    }

    void StatusEffect::SetHidden(bool value)
    {
        _hidden = value;
    }
}
