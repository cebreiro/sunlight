#include "monster_skill_data.h"

#include "sl/generator/game/data/sox/monster_skill.h"

namespace sunlight
{
    auto MonsterSkillData::CreateFrom(const sox::MonsterSkill& data) -> MonsterSkillData
    {
        MonsterSkillData result;
        result.index = data.index;
        result.routineId = data.routineId;
        result.abilityType = data.abilityType;

        result.passive = data.passive == 0;
        result.applyTargetType = static_cast<SkillTargetSelectType>(data.applyTargetType);
        result.applyDamageType = static_cast<SkillTargetingAreaType>(data.applyDamageType);
        result.damageLength = data.damageLength;
        result.damageLength2 = data.damageLength2;
        result.damageMaxCount = data.damageMaxcount;

        {
            const std::initializer_list<SkillEffectData> list{
                {
                    static_cast<SkillEffectCategory>(data.effect1), data.effect1Type,
                    data.effect1Reserved1, data.effect1Reserved2, data.effect1Reserved3,
                    data.effect1Reserved4, data.effect1Reserved5, data.effect1Reserved6,
                    data.effect1Reserved7, data.effect1Reserved8, data.effect1Reserved9,
                    data.effect1Reserved10, data.effect1Reserved11, data.effect1Reserved12,
                    data.effect1HopType, data.effect1HopA, data.effect1HopB,
                },
                {
                    static_cast<SkillEffectCategory>(data.effect2), data.effect2Type,
                    data.effect2Reserved1, data.effect2Reserved2, data.effect2Reserved3,
                    data.effect2Reserved4, data.effect2Reserved5, data.effect2Reserved6,
                    data.effect2Reserved7, data.effect2Reserved8, data.effect2Reserved9,
                    data.effect2Reserved10, data.effect2Reserved11, data.effect2Reserved12,
                    data.effect2HopType, data.effect2HopA, data.effect2HopB,
                },
                {
                    static_cast<SkillEffectCategory>(data.effect3), data.effect3Type,
                    data.effect3Reserved1, data.effect3Reserved2, data.effect3Reserved3,
                    data.effect3Reserved4, data.effect3Reserved5, data.effect3Reserved6,
                    data.effect3Reserved7, data.effect3Reserved8, data.effect3Reserved9,
                    data.effect3Reserved10, data.effect3Reserved11, data.effect3Reserved12,
                    data.effect3HopType, data.effect3HopA, data.effect3HopB,
                },
                {
                    static_cast<SkillEffectCategory>(data.effect4), data.effect4Type,
                    data.effect4Reserved1, data.effect4Reserved2, data.effect4Reserved3,
                    data.effect4Reserved4, data.effect4Reserved5, data.effect4Reserved6,
                    data.effect4Reserved7, data.effect4Reserved8, data.effect4Reserved9,
                    data.effect4Reserved10, data.effect4Reserved11, data.effect4Reserved12,
                    data.effect4HopType, data.effect4HopA, data.effect4HopB,
                }
            };

            for (const SkillEffectData& skillEffectData : list)
            {
                if (skillEffectData.category == SkillEffectCategory::None)
                {
                    continue;
                }

                result.effects.emplace_back(skillEffectData);
            }
        }

        return result;
    }
}
