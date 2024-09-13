#include "player_skill_data.h"

#include "sl/emulator/game/data/sox/skill_basic.h"

namespace sunlight
{
    auto PlayerSkillData::CreateFrom(const sox::SkillBasic& data) -> PlayerSkillData
    {
        PlayerSkillData result;
        result.index = data.index;
        result.routineId = data.routineId;
        result.abilityType = data.abilityType;

        {
            for (int32_t job : std::initializer_list<int32_t>{ data.job1, data.job2, data.job3 })
            {
                if (job != 0)
                {
                    result.jobs.push_back(static_cast<JobId>(job));
                }
            }
        }
        result.degree = data.degree;
        result.maxLevel = data.maxLevel;
        result.delayId = data.delayId;
        result.canSubJob = data.canSubjob;
        result.levelQualification = data.levelQualification;

        {
            for (const PrevSkill& skill : std::initializer_list<PrevSkill>{
                { data.ability1Qualification, data.ability1Level },
                { data.ability2Qualification, data.ability2Level }
                })
            {
                if (skill.id != 0 && skill.level != 0)
                {
                    result.prevSkills.push_back(skill);
                }
            }
        }

        result.passive = data.passive;
        result.spConsumption = data.spConsumption;
        result.spConsumptionDelay = data.spConsumptionDelay;
        result.furyOnly = data.furyOnly;
        result.requireItem = data.requireItem;
        result.requireItemCount = data.requireItemCount;
        result.motionSpeed = data.motionSpeed;
        result.needWeapon = data.needWeapon;
        result.delayType = data.delayType;
        result.delayTime = data.delayTime;
        result.useType = data.useType;
        result.applyTargetType = data.applyTargetType;
        result.applyDamageType = data.applyDamageType;
        result.damageLength = data.damageLength;
        result.damageLength2 = data.damageLength2;
        result.damageMaxCount = data.damageMaxcount;
        result.cancelable = data.cancelable;
        result.useRange = data.useRange;
        result.addWeaponRange = data.addWeaponRange;
        result.applyCasting = data.applyCasting;
        result.castingDelay = data.castingDelay;
        result.applyCharging = data.applyCharging;
        result.chargingDelay = data.chargingDelay;
        result.damageMotionType = data.damageMotionType;
        {
            std::initializer_list<PlayerSkillEffectData> list{
                {
                    data.effect1, data.effect1Type,
                    data.effect1Reserved1, data.effect1Reserved2, data.effect1Reserved3,
                    data.effect1Reserved4, data.effect1Reserved5, data.effect1Reserved6,
                    data.effect1Reserved7, data.effect1Reserved8, data.effect1Reserved9,
                    data.effect1Reserved10, data.effect1Reserved11, data.effect1Reserved12,
                    data.effect1HopType, data.effect1HopA, data.effect1HopB,
                },
                {
                    data.effect2, data.effect2Type,
                    data.effect2Reserved1, data.effect2Reserved2, data.effect2Reserved3,
                    data.effect2Reserved4, data.effect2Reserved5, data.effect2Reserved6,
                    data.effect2Reserved7, data.effect2Reserved8, data.effect2Reserved9,
                    data.effect2Reserved10, data.effect2Reserved11, data.effect2Reserved12,
                    data.effect2HopType, data.effect2HopA, data.effect2HopB,
                },
                {
                    data.effect3, data.effect3Type,
                    data.effect3Reserved1, data.effect3Reserved2, data.effect3Reserved3,
                    data.effect3Reserved4, data.effect3Reserved5, data.effect3Reserved6,
                    data.effect3Reserved7, data.effect3Reserved8, data.effect3Reserved9,
                    data.effect3Reserved10, data.effect3Reserved11, data.effect3Reserved12,
                    data.effect3HopType, data.effect3HopA, data.effect3HopB,
                },
                {
                    data.effect4, data.effect4Type,
                    data.effect4Reserved1, data.effect4Reserved2, data.effect4Reserved3,
                    data.effect4Reserved4, data.effect4Reserved5, data.effect4Reserved6,
                    data.effect4Reserved7, data.effect4Reserved8, data.effect4Reserved9,
                    data.effect4Reserved10, data.effect4Reserved11, data.effect4Reserved12,
                    data.effect4HopType, data.effect4HopA, data.effect4HopB,
                }
            };

            for (const auto& effect : list)
            {
                if (effect.category != 0)
                {
                    result.effects.push_back(effect);
                }
            }
        }

        return result;
    }
}
