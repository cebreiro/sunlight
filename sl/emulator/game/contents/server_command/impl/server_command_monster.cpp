#include "server_command_monster.h"

#include "sl/emulator/game/component/scene_object_component.h"
#include "sl/emulator/game/entity/game_monster.h"
#include "sl/emulator/game/entity/game_player.h"
#include "sl/emulator/game/system/scene_object_system.h"
#include "sl/emulator/game/system/server_command_system.h"
#include "sl/emulator/game/zone/service/game_entity_id_publisher.h"
#include "sl/emulator/service/gamedata/gamedata_provide_service.h"
#include "sl/emulator/service/gamedata/monster/monster_data_provider.h"

namespace sunlight
{
    ServerCommandMonsterSpawn::ServerCommandMonsterSpawn(ServerCommandSystem& system)
        : _system(system)
    {
    }

    auto ServerCommandMonsterSpawn::GetName() const -> std::string_view
    {
        return "monster_spawn";
    }

    auto ServerCommandMonsterSpawn::GetRequiredGmLevel() const -> int8_t
    {
        return 0;
    }

    bool ServerCommandMonsterSpawn::Execute(GamePlayer& player, int32_t mobId) const
    {
        const MonsterData* data = _system.GetServiceLocator().Get<GameDataProvideService>().GetMonsterDataProvider().Find(mobId);
        if (!data)
        {
            return false;
        }

        auto mob = std::make_shared<GameMonster>(_system.GetServiceLocator().Get<GameEntityIdPublisher>(), *data, std::nullopt);

        std::mt19937& randomEngine = _system.GetRandomEngine();
        std::uniform_real_distribution<float> distPos(10.f, 30.f);
        std::uniform_real_distribution<float> distYaw(0.f, 360.f);

        Eigen::Vector2f spawnPos = player.GetSceneObjectComponent().GetPosition();
        spawnPos.x() += distPos(randomEngine);
        spawnPos.y() += distPos(randomEngine);

        _system.Get<SceneObjectSystem>().SpawnMonster(std::move(mob), spawnPos, distYaw(randomEngine));

        return true;
    }
}
