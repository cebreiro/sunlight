#include "player_exp_system.h"

#include "sl/emulator/game/game_constant.h"
#include "sl/emulator/game/component/player_stat_component.h"
#include "sl/emulator/game/entity/game_player.h"
#include "sl/emulator/game/system/game_repository_system.h"
#include "sl/emulator/game/zone/stage.h"
#include "sl/emulator/service/gamedata/gamedata_provide_service.h"
#include "sl/emulator/service/gamedata/exp/exp_data_provider.h"

namespace sunlight
{
    PlayerExpSystem::PlayerExpSystem(const ServiceLocator& serviceLocator)
        : _serviceLocator(serviceLocator)
    {
    }

    void PlayerExpSystem::InitializeSubSystem(Stage& stage)
    {
        Add(stage.Get<GameRepositorySystem>());
    }

    bool PlayerExpSystem::Subscribe(Stage& stage)
    {
        (void)stage;

        return true;
    }

    auto PlayerExpSystem::GetName() const -> std::string_view
    {
        return "player_exp_system";
    }

    auto PlayerExpSystem::GetClassId() const -> game_system_id_type
    {
        return GameSystem::GetClassId<PlayerExpSystem>();
    }

    void PlayerExpSystem::GainExp(GamePlayer& player, int32_t exp)
    {
        GainCharacterExp(player, exp);
        GainJobExp(player, exp);
    }

    void PlayerExpSystem::GainCharacterExp(GamePlayer& player, int32_t exp)
    {
        PlayerStatComponent& statComponent = player.GetStatComponent();
        const int32_t level = statComponent.GetLevel();

        if (level >= GameConstant::PLAYER_CHARACTER_LEVEL_MAX)
        {
            return;
        }

        const ExpDataProvider& expDataProvider = _serviceLocator.Get<GameDataProvideService>().GetExpDataProvider();
        const CharacterExpData* data = expDataProvider.FindCharacterData(level);

        if (!data)
        {
            assert(false);

            return;
        }

        GameRepositorySystem& repositorySystem = Get<GameRepositorySystem>();

        const int32_t newExp = std::min(data->expMax, statComponent.GetExp() + exp);
        if (newExp >= data->expMax)
        {
            statComponent.SetLevel(level + 1);
            statComponent.SetExp(0);
            statComponent.SetStatPoint(statComponent.GetStatPoint() + GameConstant::STAT_POINT_PER_CHARACTER_LEVEL_UP);

            repositorySystem.SaveCharacterLevel(player, statComponent.GetLevel(), statComponent.GetStatPoint());
        }
        else
        {
            statComponent.SetExp(newExp);

            repositorySystem.SaveCharacterExp(player, newExp);
        }
    }

    void PlayerExpSystem::GainJobExp(GamePlayer& player, int32_t exp)
    {
        (void)player;
        (void)exp;
    }
}
