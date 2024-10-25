#include "server_command_monster.h"

#include "sl/emulator/game/component/scene_object_component.h"
#include "sl/emulator/game/entity/game_monster.h"
#include "sl/emulator/game/entity/game_player.h"
#include "sl/emulator/game/system/entity_damage_system.h"
#include "sl/emulator/game/system/entity_view_range_system.h"
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

        std::mt19937& randomEngine = _system.GetRandomEngine();
        std::uniform_real_distribution<float> distPos(10.f, 30.f);
        std::uniform_real_distribution<float> distYaw(0.f, 360.f);

        Eigen::Vector2f spawnPos = player.GetSceneObjectComponent().GetPosition();
        spawnPos.x() += distPos(randomEngine);
        spawnPos.y() += distPos(randomEngine);

        auto mob = std::make_shared<GameMonster>(_system.GetServiceLocator().Get<GameEntityIdPublisher>(), *data, spawnPos, std::nullopt);

        _system.Get<SceneObjectSystem>().SpawnMonster(std::move(mob), spawnPos, distYaw(randomEngine));

        return true;
    }

    ServerCommandMonsterKill::ServerCommandMonsterKill(ServerCommandSystem& system)
        : _system(system)
    {
    }

    auto ServerCommandMonsterKill::GetName() const -> std::string_view
    {
        return "monster_kill";
    }

    auto ServerCommandMonsterKill::GetRequiredGmLevel() const -> int8_t
    {
        return 0;
    }

    bool ServerCommandMonsterKill::Execute(GamePlayer& player) const
    {
        std::vector<game_entity_id_type> mobIds;

        _system.Get<EntityViewRangeSystem>().VisitMonster(player, [&mobIds](const GameMonster& monster)
            {
                mobIds.push_back(monster.GetId());
            });

        EntityDamageSystem& damageSystem = _system.Get<EntityDamageSystem>();

        for (const game_entity_id_type mobId : mobIds)
        {
            damageSystem.KillMonster(player, mobId);
        }

        return true;
    }

    ServerCommandMonsterRemove::ServerCommandMonsterRemove(ServerCommandSystem& system)
        : _system(system)
    {
    }

    auto ServerCommandMonsterRemove::GetName() const -> std::string_view
    {
        return "monster_remove";
    }

    auto ServerCommandMonsterRemove::GetRequiredGmLevel() const -> int8_t
    {
        return 0;
    }

    bool ServerCommandMonsterRemove::Execute(GamePlayer& player) const
    {
        std::vector<game_entity_id_type> mobIds;

        _system.Get<EntityViewRangeSystem>().VisitMonster(player, [&mobIds](const GameMonster& monster)
            {
                mobIds.push_back(monster.GetId());
            });

        SceneObjectSystem& sceneObjectSystem = _system.Get<SceneObjectSystem>();

        for (const game_entity_id_type id : mobIds)
        {
            sceneObjectSystem.RemoveMonster(id);
        }

        return true;
    }
}
