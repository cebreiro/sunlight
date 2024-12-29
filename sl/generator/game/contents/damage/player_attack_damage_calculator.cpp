#include "player_attack_damage_calculator.h"

#include "sl/generator/game/contents/damage/skill_effect_damage_type.h"
#include "sl/generator/game/component/entity_immune_component.h"
#include "sl/generator/game/component/player_game_master_component.h"
#include "sl/generator/game/entity/game_monster.h"
#include "sl/generator/game/entity/game_player.h"
#include "sl/generator/service/gamedata/skill/skill_effect_data.h"

namespace sunlight
{
    PlayerAttackDamageCalculator::PlayerAttackDamageCalculator()
        : _mt(std::random_device{}())
    {
    }

    void PlayerAttackDamageCalculator::Calculate(PlayerNormalAttackDamageCalculateResult& result, const PlayerNormalAttackDamageCalculateParam& param)
    {
        const GamePlayer& player = param.player;
        const GameMonster& target = param.target;

        result.damageType = DamageType::DamageMonster;

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

        if (const auto* immuneComponent = target.FindComponent<EntityImmuneComponent>();
            immuneComponent)
        {
            if (immuneComponent->Contains(ImmuneType::PhysicalAttack))
            {
                if (immuneComponent->Rand(ImmuneType::PhysicalAttack, _mt))
                {
                    result.damage = 0;

                    return;
                }
            }
        }

        result.damage = 35;
    }

    void PlayerAttackDamageCalculator::Calculate(PlayerSkillDamageCalculateResult& result, const PlayerSkillDamageCalculateParam& param)
    {
        const GamePlayer& player = param.player;
        const GameMonster& target = param.target;
        const SkillEffectData& skillEffectData = param.skillEffectData;

        result.damageType = DamageType::DamageMonster;

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

        const SkillEffectDamageType damageType = static_cast<SkillEffectDamageType>(skillEffectData.value10);

        if (const auto* immuneComponent = target.FindComponent<EntityImmuneComponent>();
            immuneComponent)
        {
            if (damageType == SkillEffectDamageType::Physical && immuneComponent->Contains(ImmuneType::PhysicalAttack))
            {
                if (immuneComponent->Rand(ImmuneType::PhysicalAttack, _mt))
                {
                    result.damage = 0;

                    return;
                }
            }

            if (IsMagicAttack(damageType) && immuneComponent->Contains(ImmuneType::MagicAttack))
            {
                if (immuneComponent->Rand(ImmuneType::MagicAttack, _mt))
                {
                    result.damage = 0;

                    return;
                }
            }
        }

        result.damage = 35;
    }
}
