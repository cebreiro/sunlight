#pragma once
#include <boost/container/static_vector.hpp>
#include "sl/emulator/game/contants/job/job_id.h"
#include "sl/emulator/service/gamedata/skill/player_skill_effect_data.h"

namespace sunlight::sox
{
    struct SkillBasic;
}
namespace sunlight
{
    struct PlayerSkillData
    {
        struct PrevSkill
        {
            int32_t id = 0;
            int32_t level = 0;
        };

        int32_t index = 0;
        int32_t routineId = 0;
        int32_t abilityType = 0;
        boost::container::static_vector<JobId, 3> jobs;
        int32_t degree = 0;
        int32_t maxLevel = 0;
        int32_t delayId = 0;
        int32_t canSubJob = 0;
        int32_t levelQualification = 0;
        boost::container::static_vector<PrevSkill, 2> prevSkills;
        int32_t passive = 0;
        int32_t spConsumption = 0;
        int32_t spConsumptionDelay = 0;
        int32_t furyOnly = 0;
        int32_t requireItem = 0;
        int32_t requireItemCount = 0;
        float motionSpeed = 0;
        int32_t needWeapon = 0;
        int32_t delayType = 0;
        int32_t delayTime = 0;
        int32_t useType = 0;
        int32_t applyTargetType = 0;
        int32_t applyDamageType = 0;
        int32_t damageLength = 0;
        int32_t damageLength2 = 0;
        int32_t damageMaxCount = 0;
        int32_t cancelable = 0;
        int32_t useRange = 0;
        int32_t addWeaponRange = 0;
        int32_t applyCasting = 0;
        int32_t castingDelay = 0;
        int32_t applyCharging = 0;
        int32_t chargingDelay = 0;
        int32_t damageMotionType = 0;
        boost::container::static_vector<PlayerSkillEffectData, 4> effects;

        static auto CreateFrom(const sox::SkillBasic& skillBasic) -> PlayerSkillData;
    };
}
