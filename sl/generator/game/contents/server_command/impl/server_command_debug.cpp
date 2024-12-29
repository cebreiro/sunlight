#include "server_command_debug.h"

#include "shared/execution/executor/operation/schedule.h"
#include "sl/generator/game/game_constant.h"
#include "sl/generator/game/component/entity_observable_component.h"
#include "sl/generator/game/component/player_debug_component.h"
#include "sl/generator/game/component/player_stat_component.h"
#include "sl/generator/game/component/scene_object_component.h"
#include "sl/generator/game/debug/game_debugger.h"
#include "sl/generator/game/entity/game_entity_network_id.h"
#include "sl/generator/game/entity/game_item.h"
#include "sl/generator/game/entity/game_monster.h"
#include "sl/generator/game/entity/game_player.h"
#include "sl/generator/game/message/zone_message_deliver_type.h"
#include "sl/generator/game/message/zone_message_type.h"
#include "sl/generator/game/system/entity_view_range_system.h"
#include "sl/generator/game/system/path_finding_system.h"
#include "sl/generator/game/system/scene_object_system.h"
#include "sl/generator/game/system/server_command_system.h"
#include "sl/generator/game/zone/service/game_entity_id_publisher.h"
#include "sl/generator/game/zone/service/zone_execution_service.h"
#include "sl/generator/server/packet/zone_packet_s2c.h"
#include "sl/generator/server/packet/io/sl_packet_writer.h"
#include "sl/generator/service/gamedata/gamedata_provide_service.h"
#include "sl/generator/service/gamedata/item/item_data_provider.h"

namespace sunlight
{
    ServerCommandDebugServer::ServerCommandDebugServer(ServerCommandSystem& system)
        : _system(system)
    {
    }

    auto ServerCommandDebugServer::GetName() const -> std::string_view
    {
        return "debug_server";
    }

    auto ServerCommandDebugServer::GetRequiredGmLevel() const -> int8_t
    {
        return 0;
    }

    bool ServerCommandDebugServer::Execute(GamePlayer& player, int32_t type, int32_t value) const
    {
        (void)player;

        if (GameDebugger* debugger = _system.GetServiceLocator().Find<GameDebugger>(); debugger)
        {
            if (type < 0 || type >= static_cast<int32_t>(GameDebugTypeMeta::GetSize()))
            {
                return false;
            }

            debugger->SetState(static_cast<GameDebugType>(type), value);

            return true;
        }

        return false;
    }

    ServerCommandDebugServerOff::ServerCommandDebugServerOff(ServerCommandSystem& system)
        : _system(system)
    {
    }

    auto ServerCommandDebugServerOff::GetName() const -> std::string_view
    {
        return "debug_server_off";
    }

    auto ServerCommandDebugServerOff::GetRequiredGmLevel() const -> int8_t
    {
        return 0;
    }

    bool ServerCommandDebugServerOff::Execute(GamePlayer& player) const
    {
        (void)player;

        if (GameDebugger* debugger = _system.GetServiceLocator().Find<GameDebugger>(); debugger)
        {
            debugger->Clear();

            return true;
        }

        return false;
    }

    ServerCommandDebugPlayerHP::ServerCommandDebugPlayerHP(ServerCommandSystem& system)
        : _system(system)
    {
    }

    auto ServerCommandDebugPlayerHP::GetName() const -> std::string_view
    {
        return "debug_player_hp";
    }

    auto ServerCommandDebugPlayerHP::GetRequiredGmLevel() const -> int8_t
    {
        return 0;
    }

    bool ServerCommandDebugPlayerHP::Execute(GamePlayer& player) const
    {
        PlayerDebugComponent& playerDebugComponent = player.GetDebugComponent();
        playerDebugComponent.Toggle(PlayerDebugType::HP);

        if (playerDebugComponent.IsSet(PlayerDebugType::HP))
        {
            DebugNotify(player);
        }

        return true;
    }

    void ServerCommandDebugPlayerHP::DebugNotify(GamePlayer& player) const
    {
        if (!player.GetDebugComponent().IsSet(PlayerDebugType::HP))
        {
            return;
        }

        PlayerStatComponent& statComponent = player.GetStatComponent();
        const int32_t hp = statComponent.GetFinalStat(RecoveryStatType::HP).As<int32_t>();
        const int32_t maxHP = statComponent.GetFinalStat(PlayerStatType::MaxHP).As<int32_t>();

        player.Notice(fmt::format("HP: {} / {}", hp, maxHP));

        _system.GetServiceLocator().Get<ZoneExecutionService>().AddTimer(std::chrono::milliseconds(1000), player.GetCId(), _system.GetStageId(),
            [this](GamePlayer& player)
            {
                DebugNotify(player);
            });
    }

    ServerCommandDebugPlayerSP::ServerCommandDebugPlayerSP(ServerCommandSystem& system)
        : _system(system)
    {
    }

    auto ServerCommandDebugPlayerSP::GetName() const -> std::string_view
    {
        return "debug_player_sp";
    }

    auto ServerCommandDebugPlayerSP::GetRequiredGmLevel() const -> int8_t
    {
        return 0;
    }

    bool ServerCommandDebugPlayerSP::Execute(GamePlayer& player) const
    {
        PlayerDebugComponent& playerDebugComponent = player.GetDebugComponent();
        playerDebugComponent.Toggle(PlayerDebugType::SP);

        if (playerDebugComponent.IsSet(PlayerDebugType::SP))
        {
            DebugNotify(player);
        }

        return true;
    }

    void ServerCommandDebugPlayerSP::DebugNotify(GamePlayer& player) const
    {
        PlayerStatComponent& statComponent = player.GetStatComponent();
        const int32_t sp = statComponent.GetFinalStat(RecoveryStatType::SP).As<int32_t>();
        const int32_t maxSP = statComponent.GetFinalStat(PlayerStatType::MaxSP).As<int32_t>();

        player.Notice(fmt::format("SP: {} / {}", sp, maxSP));

        _system.GetServiceLocator().Get<ZoneExecutionService>().AddTimer(std::chrono::milliseconds(1000), player.GetCId(), _system.GetStageId(),
            [this](GamePlayer& player)
            {
                DebugNotify(player);
            });
    }

    ServerCommandDebugTile::ServerCommandDebugTile(ServerCommandSystem& system)
        : _system(system)
    {
    }

    auto ServerCommandDebugTile::GetName() const -> std::string_view
    {
        return "debug_tile";
    }

    auto ServerCommandDebugTile::GetRequiredGmLevel() const -> int8_t
    {
        return 0;
    }

    bool ServerCommandDebugTile::Execute(GamePlayer& player) const
    {
        (void)player;

        PathFindingSystem* pathFindSystem = _system.Find<PathFindingSystem>();
        if (!pathFindSystem)
        {
            return false;
        }

        const ItemData* itemData = _system.GetServiceLocator().Get<GameDataProvideService>().GetItemDataProvider().Find(1);
        if (!itemData)
        {
            assert(false);

            return true;
        }

        for (int32_t y = 0; y < pathFindSystem->GetYSize(); ++y)
        {
            for (int32_t x = 0; x < pathFindSystem->GetXSize(); ++x)
            {
                const Tile& tile = pathFindSystem->GetTile(TileIndex(x, y));
                if (!tile.blocked)
                {
                    continue;
                }

                auto item = std::make_shared<GameItem>(_system.GetServiceLocator().Get<GameEntityIdPublisher>(), *itemData, 1);

                item->AddComponent(std::make_unique<SceneObjectComponent>());

                const Eigen::Vector2f& position = tile.GetPosition();

                item->GetComponent<SceneObjectComponent>().SetPosition(position);

                _system.Get<SceneObjectSystem>().SpawnItem(item, position, position, std::nullopt);
            }
        }

        return true;
    }

    ServerCommandDebugPathFind::ServerCommandDebugPathFind(ServerCommandSystem& system)
        : _system(system)
    {
    }

    auto ServerCommandDebugPathFind::GetName() const -> std::string_view
    {
        return "debug_path_find";
    }

    auto ServerCommandDebugPathFind::GetRequiredGmLevel() const -> int8_t
    {
        return 0;
    }

    bool ServerCommandDebugPathFind::Execute(GamePlayer& player, float x, float y) const
    {
        PathFindingSystem* pathFindSystem = _system.Find<PathFindingSystem>();
        if (!pathFindSystem)
        {
            return false;
        }

        std::vector<Eigen::Vector2f> result;
        if (!pathFindSystem->FindPath(result, player.GetSceneObjectComponent().GetPosition(), { x, y }))
        {
            player.Notice("find path - fail");   
        }
        else
        {
            player.Notice("find path - success");
        }

        const ItemData* itemData = _system.GetServiceLocator().Get<GameDataProvideService>().GetItemDataProvider().Find(1);
        if (!itemData)
        {
            assert(false);

            return true;
        }

        int32_t i = 1;

        for (const Eigen::Vector2f position : result)
        {
            auto item = std::make_shared<GameItem>(_system.GetServiceLocator().Get<GameEntityIdPublisher>(), *itemData,
                30000 + (i * 100));
            ++i;

            item->AddComponent(std::make_unique<SceneObjectComponent>());

            item->GetComponent<SceneObjectComponent>().SetPosition(position);

            _system.Get<SceneObjectSystem>().SpawnItem(item, position, position, std::nullopt);
        }

        return true;
    }

    ServerCommandDebugPathFindRaw::ServerCommandDebugPathFindRaw(ServerCommandSystem& system)
        : _system(system)
    {
    }

    auto ServerCommandDebugPathFindRaw::GetName() const -> std::string_view
    {
        return "debug_path_find_raw";
    }

    auto ServerCommandDebugPathFindRaw::GetRequiredGmLevel() const -> int8_t
    {
        return 0;
    }

    bool ServerCommandDebugPathFindRaw::Execute(GamePlayer& player, float x, float y) const
    {
        PathFindingSystem* pathFindSystem = _system.Find<PathFindingSystem>();
        if (!pathFindSystem)
        {
            return false;
        }

        std::vector<Eigen::Vector2f> result;
        if (!pathFindSystem->FindRawPath(result, player.GetSceneObjectComponent().GetPosition(), { x, y }))
        {
            player.Notice("find path raw - fail");
        }
        else
        {
            player.Notice("find path raw - success");
        }

        const ItemData* itemData = _system.GetServiceLocator().Get<GameDataProvideService>().GetItemDataProvider().Find(1);
        if (!itemData)
        {
            assert(false);

            return true;
        }

        int32_t i = 0;

        for (const Eigen::Vector2f position : result)
        {
            const int32_t quantity = ((i * 100) % 10000) + 1;
            auto item = std::make_shared<GameItem>(_system.GetServiceLocator().Get<GameEntityIdPublisher>(), *itemData, quantity);

            ++i;

            item->AddComponent(std::make_unique<SceneObjectComponent>());

            item->GetComponent<SceneObjectComponent>().SetPosition(position);

            _system.Get<SceneObjectSystem>().SpawnItem(item, position, position, std::nullopt);
        }

        return true;
    }

    ServerCommandDebugPathBlocked::ServerCommandDebugPathBlocked(ServerCommandSystem& system)
        : _system(system)
    {
    }

    auto ServerCommandDebugPathBlocked::GetName() const -> std::string_view
    {
        return "debug_path_find_blocked";
    }

    auto ServerCommandDebugPathBlocked::GetRequiredGmLevel() const -> int8_t
    {
        return 0;
    }

    bool ServerCommandDebugPathBlocked::Execute(GamePlayer& player, float x, float y) const
    {
        PathFindingSystem* pathFindSystem = _system.Find<PathFindingSystem>();
        if (!pathFindSystem)
        {
            return false;
        }

        const Eigen::Vector2f& playerPos = player.GetSceneObjectComponent().GetPosition();
        const bool blocked = pathFindSystem->IsBlocked(playerPos, { x, y });

        player.Notice(fmt::format("from: {}, {}, to: {}, {} is blocked: {}",
            playerPos.x(), playerPos.y(), x , y, blocked));

        return true;
    }

    ServerCommandDebugMonsterPosition::ServerCommandDebugMonsterPosition(ServerCommandSystem& system)
        : _system(system)
    {
    }

    auto ServerCommandDebugMonsterPosition::GetName() const -> std::string_view
    {
        return "debug_monster_position";
    }

    auto ServerCommandDebugMonsterPosition::GetRequiredGmLevel() const -> int8_t
    {
        return 0;
    }

    bool ServerCommandDebugMonsterPosition::Execute(GamePlayer& player) const
    {
        _system.Get<EntityViewRangeSystem>().VisitMonster(player,
            [id = player.GetCId()](GameMonster& monster)
            {
                if (!monster.HasComponent<EntityObservableComponent>())
                {
                    monster.AddComponent(std::make_unique<EntityObservableComponent>());
                }

                EntityObservableComponent& observableComponent = monster.GetComponent<EntityObservableComponent>();

                if (observableComponent.HasObserver(ObservableType::EntityPosition, id))
                {
                    observableComponent.RemoveObserver(ObservableType::EntityPosition, id);
                }
                else
                {
                    observableComponent.AddObserver(ObservableType::EntityPosition, id);
                }
            });

        return true;
    }

    auto ServerCommandDebugStringTable::GetName() const -> std::string_view
    {
        return "show_string_table";
    }

    auto ServerCommandDebugStringTable::GetRequiredGmLevel() const -> int8_t
    {
        return 0;
    }

    bool ServerCommandDebugStringTable::Execute(GamePlayer& player, int32_t index) const
    {
        SlPacketWriter writer;
        writer.Write(ZonePacketS2C::NMS_DELIVER_MESSAGE);
        writer.Write(ZoneMessageDeliverType::MSG_SC_GOB_MESSAGE);
        writer.Write<int32_t>(0);
        writer.WriteObject(GameEntityNetworkId(player).ToBuffer());
        writer.Write(ZoneMessageType::STRING_TABLE_DEBUG_SHOW);
        writer.Write<int32_t>(index);

        player.Send(writer.Flush());

        return true;
    }
}
