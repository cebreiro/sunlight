#pragma once
#include "sl/generator/game/contents/damage/monster_attack_damage_calculator_interface.h"

namespace sunlight
{
    class MonsterAttackDamageCalculator final : public IMonsterAttackDamageCalculator
    {
    public:
        void Calculate(MonsterNormalAttackDamageCalculateResult& result, const MonsterNormalAttackDamageCalculateParam& param) override;
        void Calculate(MonsterSkillDamageCalculateResult& result, const MonsterSkillDamageCalculateParam& param) override;
    };
}
