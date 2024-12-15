#include "stage.h"

#include "sl/data/map/map_prop.h"
#include "sl/data/map/map_stage.h"
#include "sl/data/map/map_stage_room.h"
#include "sl/data/map/map_stage_terrain.h"
#include "sl/generator/game/component/event_object_stage_entrance_portal_component.h"
#include "sl/generator/game/component/event_object_stage_exit_portal_component.h"
#include "sl/generator/game/component/event_object_zone_portal_component.h"
#include "sl/generator/game/component/npc_item_shop_component.h"
#include "sl/generator/game/component/scene_object_component.h"
#include "sl/generator/game/debug/game_debugger.h"
#include "sl/generator/game/entity/game_event_object.h"
#include "sl/generator/game/entity/game_npc.h"
#include "sl/generator/game/entity/game_player.h"
#include "sl/generator/game/message/character_message.h"
#include "sl/generator/game/message/zone_community_message.h"
#include "sl/generator/game/message/zone_message.h"
#include "sl/generator/game/message/zone_message_hooker.h"
#include "sl/generator/game/message/zone_message_type.h"
#include "sl/generator/game/message/zone_request.h"
#include "sl/generator/game/system/entity_ai_control_system.h"
#include "sl/generator/game/system/entity_damage_system.h"
#include "sl/generator/game/system/entity_movement_system.h"
#include "sl/generator/game/system/entity_scan_system.h"
#include "sl/generator/game/system/entity_status_effect_system.h"
#include "sl/generator/game/system/entity_view_range_system.h"
#include "sl/generator/game/system/event_bubbling_system.h"
#include "sl/generator/game/system/item_archive_system.h"
#include "sl/generator/game/system/item_trade_system.h"
#include "sl/generator/game/system/monster_drop_item_table_system.h"
#include "sl/generator/game/system/npc_shop_system.h"
#include "sl/generator/game/system/player_appearance_system.h"
#include "sl/generator/game/system/player_channel_system.h"
#include "sl/generator/game/system/player_group_system.h"
#include "sl/generator/game/system/player_index_system.h"
#include "sl/generator/game/system/player_job_system.h"
#include "sl/generator/game/system/player_profile_system.h"
#include "sl/generator/game/system/player_quest_system.h"
#include "sl/generator/game/system/entity_skill_effect_system.h"
#include "sl/generator/game/system/event_object_spawner_component.h"
#include "sl/generator/game/system/event_object_system.h"
#include "sl/generator/game/system/game_script_system.h"
#include "sl/generator/game/system/path_finding_system.h"
#include "sl/generator/game/system/player_state_system.h"
#include "sl/generator/game/system/player_stat_system.h"
#include "sl/generator/game/system/scene_object_system.h"
#include "sl/generator/game/system/server_command_system.h"
#include "sl/generator/game/time/game_time_service.h"
#include "sl/generator/game/zone/zone_execution_environment.h"
#include "sl/generator/game/zone/service/game_entity_id_publisher.h"
#include "sl/generator/game/zone/service/zone_change_service.h"
#include "sl/generator/service/config/config_provide_service.h"
#include "sl/generator/service/gamedata/gamedata_provide_service.h"
#include "sl/generator/service/gamedata/map/map_data_provider.h"
#include "sl/generator/service/gamedata/monster/monster_data_provider.h"
#include "sl/generator/service/gamedata/shop/npc_shop_data_provider.h"

namespace sunlight
{
    Stage::Stage(const ServiceLocator& serviceLocator, int32_t zoneId, const MapStage& stageData)
        : _serviceLocator(serviceLocator)
        , _zoneId(zoneId)
        , _stageData(stageData)
        , _name(fmt::format("stage_{}_{}", _zoneId, _stageData.id))
        , _zoneMessageHooker(std::make_unique<ZoneMessageHooker>(*this))
    {
        InitializeSystem();

        if (stageData.terrain)
        {
            InitializeNPC(stageData.terrain->props);
            InitializeEventObject(stageData.terrain->eventsV3);
            InitializeEventObject(stageData.terrain->eventsV5);
        }
        else if (stageData.room)
        {
            InitializeNPC(stageData.room->props);
            InitializeEventObject(stageData.room->events);
        }
        else
        {
            assert(false);
        }
    }

    Stage::~Stage()
    {
    }

    void Stage::Update()
    {
        ZoneExecutionEnvironment environment(_serviceLocator);

        SUNLIGHT_GAME_DEBUG_REPORT_INTERVAL(
            GameDebugType::SceneStatus,
            std::chrono::seconds(30),
            Get<SceneObjectSystem>().GetDebugStatus());

        for (GameSystem& system : _updateSystems | notnull::reference)
        {
            system.Update();
        }
    }

    void Stage::HandleNetworkMessage(game_client_id_type id, ZonePacketC2S opcode, UniquePtrNotNull<SlPacketReader> reader)
    {
        const auto iter = _players.find(id);
        if (iter == _players.end())
        {
            return;
        }

        SharedPtrNotNull<GamePlayer> player = iter->second;

        ZoneExecutionEnvironment environment(GetServiceLocator());

        switch (opcode)
        {
        case ZonePacketC2S::NMC_REQ_MOVE:
        case ZonePacketC2S::NMC_REQ_STOP:
        case ZonePacketC2S::NMC_REQ_SETDIRECTION:
        case ZonePacketC2S::NMC_REQ_CHANGE_ROOM:
        case ZonePacketC2S::NMC_REQ_USERINFO:
        case ZonePacketC2S::NMC_REQ_UNLOCK:
        case ZonePacketC2S::NMC_SEND_MESSAGE_TO:
        case ZonePacketC2S::NMC_SEND_LOCAL_MESSAGE:
        case ZonePacketC2S::NMC_SEND_GLOBAL_MESSAGE:
        case ZonePacketC2S::NMC_LOGIN:
        case ZonePacketC2S::NMC_LOGOUT:
        case ZonePacketC2S::UNKNOWN_0x86:
        {
            ZoneRequest request{
                .player = *player,
                .type = opcode,
                .reader = *reader,
            };

            if (!Publish(request))
            {
                SUNLIGHT_LOG_WARN(_serviceLocator,
                    fmt::format("[{}] unhandled zone request. player: {}, type: {}, buffer: {}",
                        GetName(), player->GetCId(), ToString(opcode), reader->GetBuffer().ToString()));
            }
        }
        break;
        case ZonePacketC2S::NMC_TRIGGER_EVENT:
        {
            const uint8_t eventType = reader->Read<uint8_t>();
            switch (eventType)
            {
            case 0:
            {
                const int32_t msgType = reader->Read<int32_t>();

                ZoneCommunityMessage message{
                    .player = *player,
                    .type = static_cast<ZoneMessageType>(msgType),
                    .reader = *reader,
                };

                if (!Publish(message))
                {
                    SUNLIGHT_LOG_WARN(_serviceLocator,
                        fmt::format("[{}] unhandled zone community message. player: {}, type: {}, buffer: {}",
                            GetName(), player->GetCId(), ToString(message.type), message.reader.GetBuffer().ToString()));
                }
            }
            break;
            case 1:
            {
                // stage message
                [[maybe_unused]]
                const int32_t unk = reader->Read<int32_t>();
                const auto [low, high] = reader->ReadInt64();
                const int32_t msgType = reader->Read<int32_t>();

                ZoneMessage message{
                    .player = *player,
                    .type = static_cast<ZoneMessageType>(msgType),
                    .targetId = game_entity_id_type(low),
                    .targetType = static_cast<GameEntityType>(high),
                    .reader = *reader
                };

                if (!Publish(message))
                {
                    SUNLIGHT_LOG_WARN(_serviceLocator,
                        fmt::format("[{}] unhandled zone message. player: {}, target: [{}, {}], type: {}, buffer: {}",
                            GetName(), player->GetCId(), message.targetId, ToString(message.targetType), ToString(message.type),
                            message.reader.GetBuffer().ToString()));
                }
            }
            break;
            case 3:
            {
                CharacterMessage message{
                    .player = *player,
                    .reader = *reader
                };
                message.targetName = reader->ReadString();
                message.type = static_cast<CharacterMessageType>(reader->Read<int32_t>());

                if (!Publish(message))
                {
                    SUNLIGHT_LOG_WARN(_serviceLocator,
                        fmt::format("[{}] unhandled character message. player: {}, target: {}, type: {}, buffer: {}",
                            GetName(), player->GetCId(), message.targetName, ToString(message.type),
                            message.reader.GetBuffer().ToString()));
                }
            }
            break;
            default:
                SUNLIGHT_LOG_WARN(_serviceLocator,
                    fmt::format("[{}] unhandled zone message. player: {}, event_type: {}, buffer: {}",
                        GetName(), player->GetCId(), eventType, reader->GetBuffer().ToString()));
            }
        }
        break;
        default:
            assert(false);
        }
    }

    void Stage::SpawnPlayer(SharedPtrNotNull<GamePlayer> player, StageEnterType enterType)
    {
        assert(!_players.contains(player->GetClientId()));

        _players[player->GetClientId()] = player;

        ZoneExecutionEnvironment environment(GetServiceLocator());

        Get<PlayerIndexSystem>().OnStageEnter(*player);
        Get<SceneObjectSystem>().SpawnPlayer(player, enterType);
        Get<PlayerChannelSystem>().OnStageEnter(*player, enterType);
        Get<EntityStatusEffectSystem>().OnStageEnter(*player, enterType);
        Get<EntitySkillEffectSystem>().OnStageEnter(*player, enterType);
        Get<PlayerQuestSystem>().OnStageEnter(*player, enterType);
    }

    auto Stage::DespawnPlayer(game_client_id_type clientId, StageExitType exitType) -> Future<std::shared_ptr<GamePlayer>>
    {
        std::shared_ptr<GamePlayer> player;

        const auto iter = _players.find(clientId);
        if (iter == _players.end())
        {
            co_return player;
        }

        player = std::move(iter->second);
        _players.erase(iter);

        std::optional<ZoneExecutionEnvironment> environment(GetServiceLocator());

        Get<SceneObjectSystem>().DespawnPlayer(player->GetId(), exitType);
        Get<NPCShopSystem>().OnStageExit(*player);
        Get<PlayerGroupSystem>().OnStageExit(*player);
        Get<PlayerChannelSystem>().OnStageExit(*player, exitType);
        Get<EntityStatusEffectSystem>().OnStageExit(*player, exitType);

        switch (exitType)
        {
        case StageExitType::Logout:
        case StageExitType::ZoneChange:
        {
            Get<PlayerProfileSystem>().OnZoneExit(*player);
        }
        break;
        case StageExitType::StageChange:
            break;
        }

        Get<PlayerIndexSystem>().OnStageExit(*player);
        environment.reset();

        // to ignore client network message during exit
        co_await Delay(std::chrono::milliseconds(1000));

        co_return player;
    }

    bool Stage::AddSubscriber(ZonePacketC2S type, const std::function<void(const ZoneRequest&)>& subscriber)
    {
        return _zoneRequestSubscribers.try_emplace(type, subscriber).second;
    }

    bool Stage::AddSubscriber(ZoneMessageType type, const std::function<void(const ZoneMessage&)>& subscriber)
    {
        return _zoneMessageSubscribers.try_emplace(type, subscriber).second;
    }

    bool Stage::AddSubscriber(ZoneMessageType type, const std::function<void(const ZoneCommunityMessage&)>& subscriber)
    {
        return _zoneCommunityMessageSubscribers.try_emplace(type, subscriber).second;
    }

    bool Stage::AddSubscriber(CharacterMessageType type, const std::function<void(const CharacterMessage&)>& subscriber)
    {
        return _characterMessageSubscriber.try_emplace(type, subscriber).second;
    }

    auto Stage::GetId() const -> int32_t
    {
        return _stageData.id;
    }

    auto Stage::GetName() const -> const std::string&
    {
        return _name;
    }

    auto Stage::GetServiceLocator() const -> const ServiceLocator&
    {
        return _serviceLocator;
    }

    template <typename T> requires std::derived_from<T, GameSystem>
    bool Stage::Add(SharedPtrNotNull<T> system)
    {
        assert(system);

        const auto& id = GameSystem::GetClassId<T>();

        const auto& [iter, inserted] = _systems.try_emplace(id, nullptr);
        if (inserted)
        {
            iter->second = std::move(system);

            if (iter->second->ShouldUpdate())
            {
                _updateSystems.emplace_back(iter->second.get());
            }
        }

        return inserted;
    }

    void Stage::InitializeSystem()
    {
        Add(std::make_shared<SceneObjectSystem>(_serviceLocator, _stageData.id));
        Add(std::make_shared<EntityViewRangeSystem>(_serviceLocator, _stageData));
        Add(std::make_shared<EntityMovementSystem>());
        Add(std::make_shared<ServerCommandSystem>(_serviceLocator, _zoneId, _stageData.id));
        Add(std::make_shared<ItemArchiveSystem>(_serviceLocator));
        Add(std::make_shared<PlayerStatSystem>(_serviceLocator));
        Add(std::make_shared<PlayerJobSystem>(_serviceLocator, _stageData.id));
        Add(std::make_shared<PlayerStateSystem>(_serviceLocator, _stageData, _zoneId));
        Add(std::make_shared<PlayerAppearanceSystem>(_serviceLocator));
        Add(std::make_shared<PlayerQuestSystem>(_serviceLocator));
        Add(std::make_shared<NPCShopSystem>(_serviceLocator));
        Add(std::make_shared<PlayerProfileSystem>(_serviceLocator, _zoneId));
        Add(std::make_shared<PlayerGroupSystem>(_serviceLocator, _stageData.id));
        Add(std::make_shared<ItemTradeSystem>(_serviceLocator));
        Add(std::make_shared<PlayerChannelSystem>(_serviceLocator, _zoneId));
        Add(std::make_shared<PlayerIndexSystem>());
        Add(std::make_shared<EntitySkillEffectSystem>(_serviceLocator, _stageData.id));
        Add(std::make_shared<EntityStatusEffectSystem>(_serviceLocator, _stageData.id));
        Add(std::make_shared<EntityDamageSystem>(_serviceLocator, _stageData.id));
        Add(std::make_shared<MonsterDropItemTableSystem>(_serviceLocator, _stageData.id));
        Add(std::make_shared<EventBubblingSystem>());
        Add(std::make_shared<EntityAIControlSystem>(_serviceLocator));
        Add(std::make_shared<EntityScanSystem>());
        Add(std::make_shared<EventObjectSystem>(_serviceLocator, _stageData.id));
        Add(std::make_shared<GameScriptSystem>(_serviceLocator, _stageData.id));

        if (_stageData.terrain)
        {
            Add(std::make_shared<PathFindingSystem>(_serviceLocator, *_stageData.terrain));
        }

        const auto range = _systems | std::views::values;

        for (const SharedPtrNotNull<GameSystem>& system : range)
        {
            system->InitializeSubSystem(*this);
        }

        for (const SharedPtrNotNull<GameSystem>& system : range)
        {
            system->Subscribe(*this);
        }

        for (const SharedPtrNotNull<GameSystem>& system : range)
        {
            std::vector<std::string> cyclePaths;
            if (system->HasCyclicSystemDependency(&cyclePaths))
            {
                assert(!cyclePaths.empty());

                std::ostringstream oss;

                constexpr const char* connectionStr = " - ";

                for (const std::string& path : cyclePaths)
                {
                    oss << path << connectionStr;
                }

                std::string str = oss.str();
                str.erase(str.end() - static_cast<int64_t>(::strlen(connectionStr)), str.end());

                assert(false);

                throw std::runtime_error(fmt::format("system cyclic dependency detected. cycle: {}", str));
            }
        }
    }

    void Stage::InitializeNPC(const std::vector<MapProp>& props)
    {
        const NPCShopDataProvider& npcShopDataProvider = _serviceLocator.Get<GameDataProvideService>().GetNPCShopDataProvider();
        GameEntityIdPublisher& entityIdPublisher = _serviceLocator.Get<GameEntityIdPublisher>();

        for (const MapProp& prop : props)
        {
            if (prop.type == static_cast<int32_t>(GameEntityType::NPC))
            {
                const auto [pos, yaw] = MapDataProvider::ExtractPositionAndYaw(prop.transform);

                auto sceneObjectComponent = std::make_unique<SceneObjectComponent>();
                sceneObjectComponent->SetId(entityIdPublisher.PublishSceneObjectId(GameEntityType::NPC));
                sceneObjectComponent->SetPosition(Eigen::Vector2f(pos.x(), pos.y()));
                sceneObjectComponent->SetDestPosition(sceneObjectComponent->GetPosition());
                sceneObjectComponent->SetYaw(yaw);

                auto npc = std::make_shared<GameNPC>(game_entity_id_type(prop.id), prop.pnx);
                npc->AddComponent(std::move(sceneObjectComponent));

                if (prop.unk2)
                {
                    if (const ItemShopData* itemShopData = npcShopDataProvider.FindItemShopData(prop.unk2); itemShopData)
                    {
                        npc->AddComponent(std::make_unique<NPCItemShopComponent>(*itemShopData));

                        Get<NPCShopSystem>().InitializeItemShop(*npc);
                    }

                    if (const HairShopData* hairShopData = npcShopDataProvider.FindHairShopData(prop.unk2); hairShopData)
                    {
                        
                    }
                }

                Get<SceneObjectSystem>().SpawnNPC(std::move(npc));
            }
        }
    }

    void Stage::InitializeNPC(const std::vector<MapTerrainProp>& props)
    {
        const NPCShopDataProvider& npcShopDataProvider = _serviceLocator.Get<GameDataProvideService>().GetNPCShopDataProvider();
        GameEntityIdPublisher& entityIdPublisher = _serviceLocator.Get<GameEntityIdPublisher>();

        for (const MapTerrainProp& prop : props)
        {
            if (prop.type == static_cast<int32_t>(GameEntityType::NPC))
            {
                const auto [pos, yaw] = MapDataProvider::ExtractPositionAndYaw(prop.transform);

                auto sceneObjectComponent = std::make_unique<SceneObjectComponent>();
                sceneObjectComponent->SetId(entityIdPublisher.PublishSceneObjectId(GameEntityType::NPC));
                sceneObjectComponent->SetPosition(Eigen::Vector2f(pos.x(), pos.y()));
                sceneObjectComponent->SetDestPosition(sceneObjectComponent->GetPosition());
                sceneObjectComponent->SetYaw(yaw);

                auto npc = std::make_shared<GameNPC>(game_entity_id_type(prop.id), prop.pnx);
                npc->AddComponent(std::move(sceneObjectComponent));
                npc->SetUnk1(prop.unk1);
                npc->SetUnk2(prop.unk2);

                if (prop.unk6)
                {
                    if (const ItemShopData* itemShopData = npcShopDataProvider.FindItemShopData(prop.unk6); itemShopData)
                    {
                        npc->AddComponent(std::make_unique<NPCItemShopComponent>(*itemShopData));

                        Get<NPCShopSystem>().InitializeItemShop(*npc);
                    }

                    if (const HairShopData* hairShopData = npcShopDataProvider.FindHairShopData(prop.unk6); hairShopData)
                    {

                    }
                }

                Get<SceneObjectSystem>().SpawnNPC(std::move(npc));
            }
        }
    }

    void Stage::InitializeEventObject(const std::vector<MapEventObjectV3>& events)
    {
        for (const MapEventObjectV3& event : events)
        {
            (void)event;
        }
    }

    void Stage::InitializeEventObject(const std::vector<MapEventObjectV5>& events)
    {
        bool noSpawnMonster = _serviceLocator.Get<ConfigProvideService>().GetGameConfig().noSpawnMonster;

        for (const MapEventObjectV5& event : events)
        {
            const auto& [position, yaw] = MapDataProvider::ExtractPositionAndYaw(event.transform);

            const float x = event.vector.x() / 2.f;
            const float y = event.vector.y() / 2.f;

            const Eigen::Vector2f center(position.x(), position.y());
            const Eigen::Vector2f min(position.x() - x, position.y() - y);
            const Eigen::Vector2f max(position.x() + x, position.y() + y);

            auto eventObject = std::make_shared<GameEventObject>(game_entity_id_type(event.id), center, yaw, Eigen::AlignedBox2f(min, max));

            switch (event.style)
            {
            case 2000:
            case 5000:
            {
                eventObject->AddComponent(std::make_unique<EventObjectZonePortalComponent>(event.reserved2, event.reserved1));
            }
            break;
            case 3000:
            case 4000:
            {
                eventObject->AddComponent(std::make_unique<EventObjectStageEntrancePortalComponent>(event.reserved1));
            }
            break;
            case 3001:
            case 4001:
            {
                const int32_t linkId = event.reserved1;

                eventObject->AddComponent(std::make_unique<EventObjectStageExitPortalComponent>(linkId));

                _serviceLocator.Get<ZoneChangeService>().RegisterStageExitPortal(linkId, _stageData.id, eventObject->GetId());
            }
            break;
            case 20000:
            {
                if (noSpawnMonster)
                {
                    break;
                }

                const GameDataProvideService& gameDataProvider = _serviceLocator.Get<GameDataProvideService>();
                const MapDataProvider& mapDataProvider = gameDataProvider.GetMapDataProvider();
                const MonsterDataProvider& monsterDataProvider = gameDataProvider.GetMonsterDataProvider();

                if (const NesScriptCall* scriptCall = mapDataProvider.FindNesScriptCall(_zoneId, event.id);
                    scriptCall)
                {
                    for (const auto& [type, scriptId] : scriptCall->scriptCalls)
                    {
                        if (const NesScript* script = mapDataProvider.FindNesScript(_zoneId, scriptId);
                            script && std::ssize(script->lines) >= 2)
                        {
                            auto iter = script->lines.begin();
                            for (auto next = std::next(iter); next != script->lines.end(); iter = next++)
                            {
                                if (iter->type != 1901)
                                {
                                    continue;
                                }

                                if (next->type != 1902 || std::ssize(iter->tokens) < 2 || std::ssize(next->tokens) < 3)
                                {
                                    break;
                                }

                                const NesScriptToken& mobCount = iter->tokens[0];
                                const NesScriptToken& mobId = iter->tokens[1];

                                const NesScriptToken& firstDelay = next->tokens[1];
                                const NesScriptToken& delay = next->tokens[2];

                                constexpr auto token_type = NesScriptTokenType::Int1;

                                if (mobCount.GetType() != token_type || mobId.GetType() != token_type ||
                                    firstDelay.GetType() != token_type || delay.GetType() != token_type)
                                {
                                    break;
                                }
                                const MonsterData* monsterData = monsterDataProvider.Find(mobId.Get<token_type>());
                                if (!monsterData)
                                {
                                    continue;
                                }

                                if (!eventObject->HasComponent<EventObjectSpawnerComponent>())
                                {
                                    eventObject->AddComponent(std::make_unique<EventObjectSpawnerComponent>());
                                }

                                eventObject->GetComponent<EventObjectSpawnerComponent>().AddContext(
                                    *monsterData, mobCount.Get<token_type>(),
                                    firstDelay.Get<token_type>(), delay.Get<token_type>());
                            }
                        }
                    }
                }
            }
            break;
            }

            [[maybe_unused]]
            const bool added = Get<EventObjectSystem>().AddEventObject(std::move(eventObject));
            assert(added);
        }
    }

    bool Stage::Publish(const ZoneRequest& request)
    {
        const auto iter = _zoneRequestSubscribers.find(request.type);
        if (iter == _zoneRequestSubscribers.end())
        {
            return false;
        }

        iter->second(request);

        return true;
    }

    bool Stage::Publish(const ZoneMessage& message)
    {
        if (_zoneMessageHooker->ProcessEvent(message))
        {
            return true;
        }

        const auto iter = _zoneMessageSubscribers.find(message.type);
        if (iter == _zoneMessageSubscribers.end())
        {
            return false;
        }

        iter->second(message);

        return true;
    }

    bool Stage::Publish(const ZoneCommunityMessage& message)
    {
        const auto iter = _zoneCommunityMessageSubscribers.find(message.type);
        if (iter == _zoneCommunityMessageSubscribers.end())
        {
            return false;
        }

        iter->second(message);

        return true;
    }

    bool Stage::Publish(const CharacterMessage& message)
    {
        const auto iter = _characterMessageSubscriber.find(message.type);
        if (iter == _characterMessageSubscriber.end())
        {
            return false;
        }

        iter->second(message);

        return true;
    }
}
