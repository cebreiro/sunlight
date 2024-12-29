#include "monster_attack_data.h"

#include "sl/generator/game/data/sox/monster_attack.h"

namespace sunlight
{
    MonsterAttackData::MonsterAttackData(const sox::MonsterAttack& data)
        : type(data.attackType)
        , percent(data.attackPercent)
        , range(data.attackRange)
        , damageFactor(data.attackDamageFactor)
        , divDamage(data.attackDivDamage)
        , divDamageDelay(data.attackDivDamageDelay)
        , effectRange(data.attackEffectRange)
        , reserved1(data.attackReserved1)
        , reserved2(data.attackReserved2)
        , reserved3(data.attackReserved3)
        , bulletId(data.attackBulletID)
        , attackAniPartId(data.attackAniPartID)
        , attackEndFrame(data.attackEndFrame)
        , attackDelay(data.attackDelay)
        , attackBeatFrame(data.attackBeatFrame)
        , attackFxGroupId(data.attackFxGroupID)
        , attackSoundFileId(data.attackSoundFileID)
    {
        for (const Skill& skill : std::initializer_list<Skill>{
                 {
                     data.skill1AbilityID, data.skill1Type, data.skill1Percent, data.skill1Range,
                     data.skill1CoolTime, data.skill1LV, data.skill1AniPartID,
                     data.skill1EndFrame, data.skill1Delay, data.skill1BeatFrame
                 },
                 {
                     data.skill2AbilityID, data.skill2Type, data.skill2Percent, data.skill2Range,
                     data.skill2CoolTime, data.skill2LV, data.skill2AniPartID,
                     data.skill2EndFrame, data.skill2Delay, data.skill2BeatFrame
                 },
                 {
                     data.skill3AbilityID, data.skill3Type, data.skill3Percent, data.skill3Range,
                     data.skill3CoolTime, data.skill3LV, data.skill3AniPartID,
                     data.skill3EndFrame, data.skill3Delay, data.skill3BeatFrame
                 }
            })
        {
            if (skill.id != 0 && skill.type != 0)
            {
                skills.push_back(skill);
            }
        }

        for (const Passive& passive : std::initializer_list<Passive>{
                 {
                     data.passive1ID, data.passive1Percent, data.passive1LV, data.passive1AddExp, data.passive1ExtraMoney
                 },
                 {
                     data.passive2ID, data.passive2Percent, data.passive2LV, data.passive2AddExp, data.passive2ExtraMoney
                 },
                 {
                     data.passive3ID, data.passive3Percent, data.passive3LV, data.passive3AddExp, data.passive3ExtraMoney
                 }
            })
        {
            if (passive.id != 0)
            {
                passives.push_back(passive);
            }
        }
    }
}
