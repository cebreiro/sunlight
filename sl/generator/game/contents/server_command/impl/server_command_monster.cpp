#include "server_command_monster.h"

#include "sl/generator/game/component/scene_object_component.h"
#include "sl/generator/game/entity/game_monster.h"
#include "sl/generator/game/entity/game_player.h"
#include "sl/generator/game/system/entity_ai_control_system.h"
#include "sl/generator/game/system/entity_damage_system.h"
#include "sl/generator/game/system/entity_movement_system.h"
#include "sl/generator/game/system/entity_view_range_system.h"
#include "sl/generator/game/system/scene_object_system.h"
#include "sl/generator/game/system/server_command_system.h"
#include "sl/generator/game/zone/service/game_entity_id_publisher.h"
#include "sl/generator/service/gamedata/gamedata_provide_service.h"
#include "sl/generator/service/gamedata/monster/monster_data_provider.h"

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
        std::vector<PtrNotNull<GameMonster>> targets;

        _system.Get<EntityViewRangeSystem>().VisitMonster(player, [&targets](GameMonster& monster)
            {
                targets.push_back(&monster);
            });

        EntityDamageSystem& damageSystem = _system.Get<EntityDamageSystem>();

        for (GameMonster& target : targets | notnull::reference)
        {
            damageSystem.KillMonster(player, target);
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

    ServerCommandMonsterAISuspend::ServerCommandMonsterAISuspend(ServerCommandSystem& system)
        : _system(system)
    {
    }

    auto ServerCommandMonsterAISuspend::GetName() const -> std::string_view
    {
        return "monster_ai_suspend";
    }

    auto ServerCommandMonsterAISuspend::GetRequiredGmLevel() const -> int8_t
    {
        return 0;
    }

    bool ServerCommandMonsterAISuspend::Execute(GamePlayer& player) const
    {
        EntityAIControlSystem& entityAIControlSystem = _system.Get<EntityAIControlSystem>();

        _system.Get<EntityViewRangeSystem>().VisitMonster(player,
            [&entityAIControlSystem](const GameMonster& monster)
            {
                entityAIControlSystem.SuspendMonsterControl(monster.GetId());
            });

        return true;
    }

    ServerCommandMonsterAIResume::ServerCommandMonsterAIResume(ServerCommandSystem& system)
        : _system(system)
    {
    }

    auto ServerCommandMonsterAIResume::GetName() const -> std::string_view
    {
        return "monster_ai_resume";
    }

    auto ServerCommandMonsterAIResume::GetRequiredGmLevel() const -> int8_t
    {
        return 0;
    }

    bool ServerCommandMonsterAIResume::Execute(GamePlayer& player) const
    {
        EntityAIControlSystem& entityAIControlSystem = _system.Get<EntityAIControlSystem>();

        _system.Get<EntityViewRangeSystem>().VisitMonster(player,
            [&entityAIControlSystem](const GameMonster& monster)
            {
                entityAIControlSystem.ResumeMonsterControl(monster.GetId());
            });

        return true;
    }

    ServerCommandMonsterComeHere::ServerCommandMonsterComeHere(ServerCommandSystem& system)
        : _system(system)
    {
    }

    auto ServerCommandMonsterComeHere::GetName() const -> std::string_view
    {
        return "monster_come_here";
    }

    auto ServerCommandMonsterComeHere::GetRequiredGmLevel() const -> int8_t
    {
        return 0;
    }

    bool ServerCommandMonsterComeHere::Execute(GamePlayer& player) const
    {
        EntityMovementSystem& entityMovementSystem = _system.Get<EntityMovementSystem>();

        _system.Get<EntityViewRangeSystem>().VisitMonster(player,
            [&entityMovementSystem , &player](GameMonster& monster)
            {
                entityMovementSystem.MoveToTarget(monster, player, monster.GetMoveSpeed());
            });

        return true;
    }
}
