#include "player_attack_damage_calculator.h"

#include "sl/emulator/game/component/player_game_master_component.h"
#include "sl/emulator/game/entity/game_player.h"

namespace sunlight
{
    void PlayerAttackDamageCalculator::Calculate(PlayerNormalAttackDamageCalculateResult& result, const PlayerNormalAttackDamageCalculateParam& param)
    {
        const GamePlayer& player = param.player;

        if (const PlayerGameMasterComponent* gameMasterComponent = player.FindComponent<PlayerGameMasterComponent>();
            gameMasterComponent)
        {
            if (const auto damage = gameMasterComponent->GetDamage();
                damage)
            {
                result.damage = *damage;

                return;
            }
        }

        result.damage = 35;
    }

    void PlayerAttackDamageCalculator::Calculate(PlayerSkillDamageCalculateResult& result, const PlayerSkillDamageCalculateParam& param)
    {
        const GamePlayer& player = param.player;

        if (const PlayerGameMasterComponent* gameMasterComponent = player.FindComponent<PlayerGameMasterComponent>();
            gameMasterComponent)
        {
            if (const auto damage = gameMasterComponent->GetDamage();
                damage)
            {
                result.damage = *damage;

                return;
            }
        }

        result.damage = 35;
    }
}
