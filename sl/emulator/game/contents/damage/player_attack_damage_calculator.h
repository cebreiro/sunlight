#pragma once
#include "sl/emulator/game/contents/damage/player_attack_damage_calculator_interface.h"

namespace sunlight
{
    class PlayerAttackDamageCalculator final : public IPlayerAttackDamageCalculator
    {
    public:
        void Calculate(PlayerNormalAttackDamageCalculateResult& result, const PlayerNormalAttackDamageCalculateParam& param) override;
        void Calculate(PlayerSkillDamageCalculateResult& result, const PlayerSkillDamageCalculateParam& param) override;
    };
}
