#include "status_effect_stat_increase.h"

#include "sl/generator/game/component/player_stat_component.h"
#include "sl/generator/game/contents/status_effect/status_effect.h"
#include "sl/generator/game/entity/game_entity.h"
#include "sl/generator/game/entity/game_player.h"
#include "sl/generator/game/system/entity_status_effect_system.h"
#include "sl/generator/game/system/player_stat_system.h"

namespace sunlight
{
    auto StatusEffectStatIncreaseHandler::GetType() const -> StatusEffectType
    {
        return StatusEffectType::StatIncrease;
    }

    void StatusEffectStatIncreaseHandler::Apply(EntityStatusEffectSystem& system, GameEntity& entity, StatusEffect& statusEffect) const
    {
        (void)system;

        if (entity.GetType() == GameEntityType::Player)
        {
            GamePlayer* player = entity.Cast<GamePlayer>();

            PlayerStatComponent& statComponent = player->GetStatComponent();

            const PlayerStatType statType = static_cast<PlayerStatType>(statusEffect.GetStatType());

            const int32_t statValue = statusEffect.GetStatValue();
            const int32_t statPercentage = statusEffect.GetStatPercentageValue();

            if (statValue != 0)
            {
                statComponent.AddStat(statType, StatOriginType::StatusEffect, statValue);
            }

            if (statPercentage != 0)
            {
                statComponent.AddStat(statType, StatOriginType::StatusEffectPercentage, static_cast<double>(statPercentage) / 100.0);
            }

            system.Get<PlayerStatSystem>().UpdateRegenStat(*player);
        }
    }

    void StatusEffectStatIncreaseHandler::Revert(EntityStatusEffectSystem& system, GameEntity& entity, StatusEffect& statusEffect) const
    {
        (void)system;

        if (entity.GetType() == GameEntityType::Player)
        {
            GamePlayer* player = entity.Cast<GamePlayer>();

            PlayerStatComponent& statComponent = player->GetStatComponent();

            const PlayerStatType statType = static_cast<PlayerStatType>(statusEffect.GetStatType());

            const int32_t statValue = statusEffect.GetStatValue();
            const int32_t statPercentage = statusEffect.GetStatPercentageValue();

            if (statValue != 0)
            {
                statComponent.AddStat(statType, StatOriginType::StatusEffect, -statValue);
            }

            if (statPercentage != 0)
            {
                statComponent.AddStat(statType, StatOriginType::StatusEffectPercentage, -(static_cast<double>(statPercentage) / 100.0));
            }

            system.Get<PlayerStatSystem>().UpdateRegenStat(*player);
        }
    }
}
