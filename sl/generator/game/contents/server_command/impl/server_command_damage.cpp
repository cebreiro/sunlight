#include "server_command_damage.h"

#include "sl/generator/game/component/player_game_master_component.h"
#include "sl/generator/game/entity/game_player.h"

namespace sunlight
{
    auto ServerCommandDamageSet::GetName() const -> std::string_view
    {
        return "damage_set";
    }

    auto ServerCommandDamageSet::GetRequiredGmLevel() const -> int8_t
    {
        return 0;
    }

    bool ServerCommandDamageSet::Execute(GamePlayer& player, int32_t value) const
    {
        if (!player.HasComponent<PlayerGameMasterComponent>())
        {
            player.AddComponent(std::make_unique<PlayerGameMasterComponent>());
        }

        player.GetComponent<PlayerGameMasterComponent>().SetDamage(value);

        return true;
    }

    auto ServerCommandDamageClear::GetName() const -> std::string_view
    {
        return "damage_clear";
    }

    auto ServerCommandDamageClear::GetRequiredGmLevel() const -> int8_t
    {
        return 0;
    }

    bool ServerCommandDamageClear::Execute(GamePlayer& player) const
    {
        PlayerGameMasterComponent* gameMasterComponent = player.FindComponent<PlayerGameMasterComponent>();
        if (!gameMasterComponent)
        {
            return true;
        }

        gameMasterComponent->ClearDamage();

        return true;
    }
}
