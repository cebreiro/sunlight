#include "player_attack_damage_calculator.h"

namespace sunlight
{
    void PlayerAttackDamageCalculator::Calculate(PlayerNormalAttackDamageCalculateResult& result, const PlayerNormalAttackDamageCalculateParam& param)
    {
        (void)param;

        result.damage = 10;
    }

    void PlayerAttackDamageCalculator::Calculate(PlayerSkillDamageCalculateResult& result, const PlayerSkillDamageCalculateParam& param)
    {
        (void)param;

        result.damage = 10;
    }
}
