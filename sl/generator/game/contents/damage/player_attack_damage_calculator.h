#pragma once
#include "sl/generator/game/contents/damage/player_attack_damage_calculator_interface.h"

namespace sunlight
{
    class PlayerAttackDamageCalculator final : public IPlayerAttackDamageCalculator
    {
    public:
        PlayerAttackDamageCalculator();

        void Calculate(PlayerNormalAttackDamageCalculateResult& result, const PlayerNormalAttackDamageCalculateParam& param) override;
        void Calculate(PlayerSkillDamageCalculateResult& result, const PlayerSkillDamageCalculateParam& param) override;

    private:
        std::mt19937 _mt;
    };
}
