#pragma once
#include <boost/container/static_vector.hpp>

namespace sunlight::sox
{
    struct MonsterAttack;
}

namespace sunlight
{
    struct MonsterAttackData
    {
        struct Skill
        {
            int32_t id;
            int32_t type;
            int32_t percent;
            int32_t range;
            int32_t coolTime;
            int32_t level;
            int32_t skillAniPartId;
            int32_t skillEndFrame;
            int32_t skillDelay;
            int32_t skillBeatFrame;
        };

        struct Passive
        {
            int32_t id;
            int32_t percent;
            int32_t level;
            int32_t addExp;
            int32_t extraMoney;
        };

        MonsterAttackData() = default;
        explicit MonsterAttackData(const sox::MonsterAttack& data);

        int32_t type;
        int32_t percent;
        int32_t range;
        float damageFactor;
        int32_t divDamage;
        int32_t divDamageDelay;
        int32_t effectRange;
        int32_t reserved1;
        int32_t reserved2;
        int32_t reserved3;
        int32_t bulletId;
        int32_t attackAniPartId;
        int32_t attackEndFrame;
        int32_t attackDelay;
        int32_t attackBeatFrame;
        int32_t attackFxGroupId;
        int32_t attackSoundFileId;
        boost::container::static_vector<Skill, 3> skills;
        boost::container::static_vector<Passive, 3> passives;
    };
}
