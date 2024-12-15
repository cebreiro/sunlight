#pragma once
#include "sl/generator/game/contents/damage/damage_result_type.h"

namespace sunlight
{
    struct MonsterAttackData;
    struct MonsterSkillData;
    struct SkillEffectData;

    class GameMonster;
    class GameEntity;
}

namespace sunlight
{
    struct MonsterNormalAttackDamageCalculateParam
    {
        const GameMonster& monster;
        const GameEntity& target;

        const MonsterAttackData& attackData;
    };

    struct MonsterNormalAttackDamageCalculateResult
    {
        DamageType damageType = {};
        int32_t damage = 0;
    };

    struct MonsterSkillDamageCalculateParam
    {
        const GameMonster& monster;
        const GameEntity& target;

        const MonsterSkillData& skillData;
        const SkillEffectData& skillEffectData;
    };

    struct MonsterSkillDamageCalculateResult
    {
        DamageType damageType = {};
        int32_t damage = 0;
    };

    class IMonsterAttackDamageCalculator
    {
    public:
        virtual ~IMonsterAttackDamageCalculator() = default;

        virtual void Calculate(MonsterNormalAttackDamageCalculateResult& result, const MonsterNormalAttackDamageCalculateParam& param) = 0;
        virtual void Calculate(MonsterSkillDamageCalculateResult& result, const MonsterSkillDamageCalculateParam& param) = 0;
    };
}
