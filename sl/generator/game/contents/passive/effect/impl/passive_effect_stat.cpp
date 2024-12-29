#include "passive_effect_stat.h"

#include "sl/generator/service/gamedata/skill/skill_effect_data.h"

namespace sunlight
{
    PassiveEffectStat::PassiveEffectStat(const SkillEffectData& data)
        : _data(data)
    {
    }

    auto PassiveEffectStat::GetType() const -> PassiveEffectType
    {
        return TYPE;
    }

    auto PassiveEffectStat::GetData() const -> const SkillEffectData&
    {
        return _data;
    }

    auto PassiveEffectStat::GetStatType() const -> int32_t
    {
        return _data.type;
    }

    auto PassiveEffectStat::GetStatValue() const -> int32_t
    {
        return _statValue;
    }

    auto PassiveEffectStat::GetStatPercentageValue() const -> double
    {
        return _statPercentageValue;
    }

    void PassiveEffectStat::SetStatValue(int32_t value)
    {
        _statValue = value;
    }

    void PassiveEffectStat::SetStatPercentageValue(double value)
    {
        _statPercentageValue = value;
    }
}
