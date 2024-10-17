#include "passive_effect_stat.h"

#include "sl/emulator/service/gamedata/skill/skill_effect_data.h"

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
}
