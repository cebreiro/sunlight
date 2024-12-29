#include "monster_attack_damage_calculator.h"

namespace sunlight
{
    void MonsterAttackDamageCalculator::Calculate(MonsterNormalAttackDamageCalculateResult& result, const MonsterNormalAttackDamageCalculateParam& param)
    {
        (void)param;

        result.damageType = DamageType::DamagePlayer;
        result.damage = 100;
    }

    void MonsterAttackDamageCalculator::Calculate(MonsterSkillDamageCalculateResult& result, const MonsterSkillDamageCalculateParam& param)
    {
        (void)param;

        result.damageType = DamageType::DamagePlayer;
        result.damage = 100;
    }
}
