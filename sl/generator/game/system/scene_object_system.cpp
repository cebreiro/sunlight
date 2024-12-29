#include "scene_object_system.h"

#include "sl/generator/game/component/player_appearance_component.h"
#include "sl/generator/game/component/player_group_component.h"
#include "sl/generator/game/component/player_npc_script_component.h"
#include "sl/generator/game/component/player_stat_component.h"
#include "sl/generator/game/component/scene_object_component.h"
#include "sl/generator/game/contents/group/item_mix_prop_item_mapping.h"
#include "sl/generator/game/contents/state/game_entity_state.h"
#include "sl/generator/game/entity/game_item.h"
#include "sl/generator/game/entity/game_monster.h"
#include "sl/generator/game/entity/game_npc.h"
#include "sl/generator/game/entity/game_player.h"
#include "sl/generator/game/entity/game_stored_item.h"
#include "sl/generator/game/message/zone_message.h"
#include "sl/generator/game/message/zone_request.h"
#include "sl/generator/game/message/creator/game_player_message_creator.h"
#include "sl/generator/game/message/creator/scene_object_message_creator.h"
#include "sl/generator/game/system/entity_view_range_system.h"
#include "sl/generator/game/system/entity_movement_system.h"
#include "sl/generator/game/system/event_bubbling_system.h"
#include "sl/generator/game/system/player_appearance_system.h"
#include "sl/generator/game/system/player_quest_system.h"
#include "sl/generator/game/system/player_stat_system.h"
#include "sl/generator/game/system/event_bubbling/monster_event_bubbling.h"
#include "sl/generator/game/zone/stage.h"
#include "sl/generator/game/zone/service/game_entity_id_publisher.h"
#include "sl/generator/game/zone/service/zone_execution_service.h"
#include "sl/generator/server/packet/creator/zone_packet_s2c_creator.h"

namespace sunlight
{
    SceneObjectSystem::SceneObjectSystem(const ServiceLocator& serviceLocator, int32_t stageId)
        : _serviceLocator(serviceLocator)
        , _stageId(stageId)
    {
    }

    SceneObjectSystem::~SceneObjectSystem()
    {
    }

    void SceneObjectSystem::InitializeSubSystem(Stage& stage)
    {
        Add(stage.Get<EntityViewRangeSystem>());
        Add(stage.Get<EntityMovementSystem>());
        Add(stage.Get<PlayerStatSystem>());
        Add(stage.Get<PlayerAppearanceSystem>());
        Add(stage.Get<EventBubblingSystem>());
    }

    bool SceneObjectSystem::Subscribe(Stage& stage)
    {
        if (!stage.AddSubscriber(ZoneMessageType::REQUEST_ALL_STATE,
            std::bind_front(&SceneObjectSystem::HandlePlayerAllState, this)))
        {
            return false;
        }

        if (!stage.AddSubscriber(ZoneMessageType::REQUESTITEMSTRUCT,
            std::bind_front(&SceneObjectSystem::HandleRequestItemStructure, this)))
        {
            return false;
        }

        if (!stage.AddSubscriber(ZoneMessageType::NPC_SETDIRECTION,
            std::bind_front(&SceneObjectSystem::HandleNPCDirectionSet, this)))
        {
            return false;
        }

        if (!stage.AddSubscriber(ZonePacketC2S::NMC_REQ_SETDIRECTION,
            std::bind_front(&SceneObjectSystem::HandleDirectionSet, this)))
        {
            return false;
        }

        return true;
    }

    auto SceneObjectSystem::GetName() const -> std::string_view
    {
        return "scene_object_system";
    }

    auto SceneObjectSystem::GetClassId() const -> game_system_id_type
    {
        return GameSystem::GetClassId<SceneObjectSystem>();
    }

    bool SceneObjectSystem::Contains(GameEntityType type, game_entity_id_type id) const
    {
        return FindEntity(type, id) != nullptr;
    }

    void SceneObjectSystem::SpawnPlayer(SharedPtrNotNull<GamePlayer> player, StageEnterType enterType)
    {
        assert(!_entityIdIndex.contains(player->GetId()));

        _entities[player->GetType()][player->GetId()] = player;
        _entityIdIndex[player->GetId()] = player.get();

        Get<PlayerStatSystem>().OnInitialize(*player);
        Get<PlayerAppearanceSystem>().UpdateEquipmentAppearance(*player);

        SceneObjectComponent& sceneObjectComponent = player->GetSceneObjectComponent();

        switch (enterType)
        {
        case StageEnterType::Login:
        {
            sceneObjectComponent.SetId(_serviceLocator.Get<GameEntityIdPublisher>().PublishSceneObjectId(player->GetType()));

            player->Defer(ZonePacketS2CCreator::CreateLoginAccept(*player, _stageId));
        }
        break;
        case StageEnterType::StageChange:
        {
            player->Send(ZonePacketS2CCreator::CreateObjectRoomChange(_stageId));
            player->Defer(ZonePacketS2CCreator::CreateObjectForceMove(*player));
        }
        break;
        }

        player->Defer(ZonePacketS2CCreator::CreateObjectVisibleRange(1400.f));

        EntityViewRangeSystem& viewRangeSystem = Get<EntityViewRangeSystem>();

        viewRangeSystem.VisitEntity(sceneObjectComponent.GetPosition(),
            [&](GameEntity& entity)
            {
                switch (entity.GetType())
                {
                case GameEntityType::Player:
                {
                    GamePlayer& other = *entity.Cast<GamePlayer>();

                    other.Defer(ZonePacketS2CCreator::CreateObjectMove(*player));
                    other.Defer(SceneObjectPacketCreator::CreateInformation(*player, true));
                    other.Defer(GamePlayerMessageCreator::CreateRemotePlayerState(*player));

                    if (const PlayerAppearanceComponent& appearanceComponent = player->GetAppearanceComponent();
                        appearanceComponent.GetHatModelId() != 0)
                    {
                        other.Defer(GamePlayerMessageCreator::CreatePlayerHairColorChange(*player, appearanceComponent.GetHairColor()));
                    }
                    other.FlushDeferred();

                    player->Defer(ZonePacketS2CCreator::CreateObjectMove(other));
                    player->Defer(SceneObjectPacketCreator::CreateInformation(other, false));
                    player->Defer(GamePlayerMessageCreator::CreateRemotePlayerState(other));

                    if (const PlayerAppearanceComponent& appearanceComponent = other.GetAppearanceComponent();
                        appearanceComponent.GetHatModelId() != 0)
                    {
                        player->Defer(GamePlayerMessageCreator::CreatePlayerHairColorChange(other, appearanceComponent.GetHairColor()));
                    }

                    if (const PlayerGroupComponent& otherGroupComponent = other.GetGroupComponent();
                        otherGroupComponent.HasGroup())
                    {
                        player->Defer(GamePlayerMessageCreator::CreatePlayerStateProposition(other, otherGroupComponent.GetGroupState()));

                        if (otherGroupComponent.GetGroupType() == GameGroupType::StreetVendor)
                        {
                            player->Defer(SceneObjectPacketCreator::CreateState(other, GameEntityState{ .type = GameEntityStateType::StreetVendor }));
                        }
                    }
                }
                break;
                case GameEntityType::NPC:
                {
                    const GameNPC& npc = *entity.Cast<GameNPC>();

                    player->Defer(ZonePacketS2CCreator::CreateObjectMove(npc));
                    player->Defer(SceneObjectPacketCreator::CreateInformation(npc));
                }
                break;
                case GameEntityType::Item:
                {
                    const GameItem& item = *entity.Cast<GameItem>();

                    player->Defer(ZonePacketS2CCreator::CreateObjectMove(item));
                    player->Defer(SceneObjectPacketCreator::CreateInformation(item));
                    player->Defer(SceneObjectPacketCreator::CreateItemDisplay(item, player->GetCId()));
                }
                break;
                case GameEntityType::ItemChild:
                {
                    const GameStoredItem& item = *entity.Cast<GameStoredItem>();

                    player->Defer(ZonePacketS2CCreator::CreateObjectMove(item));
                    player->Defer(SceneObjectPacketCreator::CreateInformation(item));
                    player->Defer(SceneObjectPacketCreator::CreateItemDisplay(item));
                }
                break;
                case GameEntityType::Enemy:
                {
                    const GameMonster& monster = *entity.Cast<GameMonster>();

                    player->Defer(ZonePacketS2CCreator::CreateObjectMove(monster));
                    player->Defer(SceneObjectPacketCreator::CreateInformation(monster, false));
                }
                break;
                default:
                    assert(false);
                }
            });

        viewRangeSystem.Add(*player);

        if (player->HasDeferred())
        {
            player->FlushDeferred();
        }
    }

    bool SceneObjectSystem::SpawnNPC(SharedPtrNotNull<GameNPC> npc)
    {
        assert(npc->HasComponent<SceneObjectComponent>());

        auto& npcs = _entities[npc->GetType()];
        if (npcs.contains(npc->GetId()))
        {
            return false;
        }

        npcs[npc->GetId()] = npc;
        _entityIdIndex[npc->GetId()] = npc.get();

        const Eigen::Vector2f& position = npc->GetComponent<SceneObjectComponent>().GetPosition();

        EntityViewRangeSystem& viewRangeSystem = Get<EntityViewRangeSystem>();

        viewRangeSystem.VisitPlayer(position, [&](GamePlayer& player)
            {
                player.Defer(ZonePacketS2CCreator::CreateObjectMove(*npc));
                player.Defer(SceneObjectPacketCreator::CreateInformation(*npc));

                player.FlushDeferred();
            });
        viewRangeSystem.Add(*npc);

        return true;
    }

    void SceneObjectSystem::SpawnItem(SharedPtrNotNull<GameItem> item, Eigen::Vector2f originPos, Eigen::Vector2f destPos, std::optional<std::chrono::seconds> keepDuration)
    {
        assert(!_entityIdIndex.contains(item->GetId()));

        if (!item->HasComponent<SceneObjectComponent>())
        {
            item->AddComponent(std::make_unique<SceneObjectComponent>());
        }

        _entities[item->GetType()][item->GetId()] = item;
        _entityIdIndex[item->GetId()] = item.get();

        SceneObjectComponent& sceneObjectComponent = item->GetComponent<SceneObjectComponent>();
        sceneObjectComponent.SetId(_serviceLocator.Get<GameEntityIdPublisher>().PublishSceneObjectId(item->GetType()));
        sceneObjectComponent.SetPosition(destPos);
        sceneObjectComponent.SetDestPosition(destPos);

        EntityViewRangeSystem& viewRangeSystem = Get<EntityViewRangeSystem>();

        viewRangeSystem.VisitPlayer(destPos, [&](GamePlayer& player)
            {
                player.Defer(ZonePacketS2CCreator::CreateObjectMove(*item));
                player.Defer(SceneObjectPacketCreator::CreateInformation(*item));
                player.Defer(SceneObjectPacketCreator::CreateItemSpawn(*item, player.GetCId(), originPos));

                player.FlushDeferred();
            });
        viewRangeSystem.Add(*item);

        if (keepDuration.has_value())
        {
            _serviceLocator.Get<ZoneExecutionService>().AddTimer(*keepDuration,
                [this, itemId = item->GetId(), type = item->GetType()]()
                {
                    const GameEntity* entity = FindEntity(type, itemId);
                    if (!entity || entity->GetId().GetRecycleSequence() != itemId.GetRecycleSequence())
                    {
                        return;
                    }

                    RemoveItem(itemId);
                });
        }
    }

    void SceneObjectSystem::SpawnItem(SharedPtrNotNull<GameStoredItem> item)
    {
        assert(!_entityIdIndex.contains(item->GetId()));
        assert(item->HasComponent<SceneObjectComponent>());

        _entities[item->GetType()][item->GetId()] = item;
        _entityIdIndex[item->GetId()] = item.get();

        SceneObjectComponent& sceneObjectComponent = item->GetComponent<SceneObjectComponent>();
        sceneObjectComponent.SetId(_serviceLocator.Get<GameEntityIdPublisher>().PublishSceneObjectId(item->GetType()));

        EntityViewRangeSystem& viewRangeSystem = Get<EntityViewRangeSystem>();

        viewRangeSystem.VisitPlayer(sceneObjectComponent.GetPosition(), [&](GamePlayer& player)
            {
                player.Defer(ZonePacketS2CCreator::CreateObjectMove(*item));
                player.Defer(SceneObjectPacketCreator::CreateInformation(*item));
                player.Defer(SceneObjectPacketCreator::CreateItemDisplay(*item));

                player.FlushDeferred();
            });
        viewRangeSystem.Add(*item);
    }

    void SceneObjectSystem::SpawnMonster(SharedPtrNotNull<GameMonster> monster, Eigen::Vector2f pos, float yaw)
    {
        assert(!_entityIdIndex.contains(monster->GetId()));

        _entities[monster->GetType()][monster->GetId()] = monster;
        _entityIdIndex[monster->GetId()] = monster.get();

        SceneObjectComponent& sceneObjectComponent = monster->GetSceneObjectComponent();
        sceneObjectComponent.SetId(_serviceLocator.Get<GameEntityIdPublisher>().PublishSceneObjectId(monster->GetType()));
        sceneObjectComponent.SetPosition(pos);
        sceneObjectComponent.SetDestPosition(pos);
        sceneObjectComponent.SetYaw(yaw);

        EntityViewRangeSystem& viewRangeSystem = Get<EntityViewRangeSystem>();

        viewRangeSystem.VisitPlayer(pos, [&](GamePlayer& player)
            {
                player.Defer(ZonePacketS2CCreator::CreateObjectMove(*monster));
                player.Defer(SceneObjectPacketCreator::CreateInformation(*monster, true));

                player.FlushDeferred();
            });
        viewRangeSystem.Add(*monster);

        Get<EventBubblingSystem>().Publish(EventBubblingMonsterSpawn{ .monster = monster.get() });
    }

    bool SceneObjectSystem::DespawnPlayer(game_entity_id_type id, StageExitType exitType)
    {
        const auto iter1 = _entities.find(GameEntityType::Player);
        if (iter1 == _entities.end())
        {
            return false;
        }

        const auto iter2 = iter1->second.find(id);
        if (iter2 == iter1->second.end())
        {
            return false;
        }

        GamePlayer& player = *iter2->second->Cast<GamePlayer>();

        Get<EntityViewRangeSystem>().VisitPlayer(player,
            [&player, exitType](GamePlayer& visited)
            {
                if (&player == &visited)
                {
                    return;
                }

                if (player.IsDead())
                {
                    visited.Send(ZonePacketS2CCreator::CreateObjectLeave(player));
                }
                else
                {
                    switch (exitType)
                    {
                    case StageExitType::Logout:
                    {
                        visited.Send(SceneObjectPacketCreator::CreateState(player, GameEntityState{ .type = GameEntityStateType::Leaving, }));
                    }
                    break;
                    case StageExitType::ZoneChange:
                    {
                        visited.Send(SceneObjectPacketCreator::CreateState(player, GameEntityState{ .type = GameEntityStateType::ChangingZone, }));
                    }
                    break;
                    case StageExitType::StageChange:
                    {
                        visited.Send(SceneObjectPacketCreator::CreateState(player, GameEntityState{ .type = GameEntityStateType::ChangingRoom, }));
                    }
                    break;
                    }
                }
            });

        Get<EntityViewRangeSystem>().Remove(player);
        Get<EntityMovementSystem>().Remove(player.GetId());

        _entityIdIndex.erase(player.GetId());
        iter1->second.erase(iter2);

        return true;
    }

    void SceneObjectSystem::RemoveItem(game_entity_id_type id)
    {
        const auto iter1 = _entities.find(GameEntityType::Item);
        if (iter1 == _entities.end())
        {
            assert(false);

            return;
        }

        const auto iter2 = iter1->second.find(id);
        if (iter2 == iter1->second.end())
        {
            assert(false);

            return;
        }

        GameEntity& entity = *iter2->second;
        EntityViewRangeSystem& viewRangeSystem = Get<EntityViewRangeSystem>();

        viewRangeSystem.Broadcast(entity, ZonePacketS2CCreator::CreateObjectLeave(entity), false);
        viewRangeSystem.Remove(entity);

        _entityIdIndex.erase(entity.GetId());
        iter1->second.erase(iter2);
    }

    bool SceneObjectSystem::RemoveStoredItem(game_entity_id_type id)
    {
        const auto iter1 = _entities.find(GameStoredItem::TYPE);
        if (iter1 == _entities.end())
        {
            return false;
        }

        const auto iter2 = iter1->second.find(id);
        if (iter2 == iter1->second.end())
        {
            return false;
        }

        GameEntity& entity = *iter2->second;
        EntityViewRangeSystem& viewRangeSystem = Get<EntityViewRangeSystem>();

        viewRangeSystem.Broadcast(entity, ZonePacketS2CCreator::CreateObjectLeave(entity), false);
        viewRangeSystem.Remove(entity);

        _entityIdIndex.erase(entity.GetId());
        iter1->second.erase(iter2);

        return true;
    }

    bool SceneObjectSystem::RemoveMonster(game_entity_id_type id)
    {
        const auto iter1 = _entities.find(GameMonster::TYPE);
        if (iter1 == _entities.end())
        {
            return false;
        }

        const auto iter2 = iter1->second.find(id);
        if (iter2 == iter1->second.end())
        {
            return false;
        }

        GameEntity& entity = *iter2->second;
        EntityViewRangeSystem& viewRangeSystem = Get<EntityViewRangeSystem>();

        viewRangeSystem.Broadcast(entity, ZonePacketS2CCreator::CreateObjectLeave(entity), false);
        viewRangeSystem.Remove(entity);
        Get<EntityMovementSystem>().Remove(id);

        Get<EventBubblingSystem>().Publish(EventBubblingMonsterDespawn{ .monster = entity.Cast<GameMonster>() });

        _entityIdIndex.erase(entity.GetId());
        iter1->second.erase(iter2);

        return true;
    }

    auto SceneObjectSystem::FindEntity(game_entity_id_type id) -> GameEntity*
    {
        const auto iter = _entityIdIndex.find(id);

        return iter != _entityIdIndex.end() ? iter->second : nullptr;
    }

    auto SceneObjectSystem::FindEntity(game_entity_id_type id) const -> const GameEntity*
    {
        const auto iter = _entityIdIndex.find(id);

        return iter != _entityIdIndex.end() ? iter->second : nullptr;
    }

    auto SceneObjectSystem::FindEntity(GameEntityType type, game_entity_id_type id) -> GameEntity*
    {
        const auto iter1 = _entities.find(type);
        if (iter1 == _entities.end())
        {
            return nullptr;
        }

        const auto iter2 = iter1->second.find(id);
        if (iter2 == iter1->second.end())
        {
            return nullptr;
        }

        return iter2->second.get();
    }

    auto SceneObjectSystem::FindEntity(GameEntityType type, game_entity_id_type id) const -> const GameEntity*
    {
        const auto iter1 = _entities.find(type);
        if (iter1 == _entities.end())
        {
            return nullptr;
        }

        const auto iter2 = iter1->second.find(id);
        if (iter2 == iter1->second.end())
        {
            return nullptr;
        }

        return iter2->second.get();
    }

    auto SceneObjectSystem::FindEntityShared(GameEntityType type, game_entity_id_type id) -> std::shared_ptr<GameEntity>
    {
        const auto iter1 = _entities.find(type);
        if (iter1 == _entities.end())
        {
            return nullptr;
        }

        const auto iter2 = iter1->second.find(id);
        if (iter2 == iter1->second.end())
        {
            return nullptr;
        }

        return iter2->second;

    }

    auto SceneObjectSystem::FindEntityShared(GameEntityType type, game_entity_id_type id) const -> std::shared_ptr<const GameEntity>
    {
        const auto iter1 = _entities.find(type);
        if (iter1 == _entities.end())
        {
            return nullptr;
        }

        const auto iter2 = iter1->second.find(id);
        if (iter2 == iter1->second.end())
        {
            return nullptr;
        }

        return iter2->second;
    }

    auto SceneObjectSystem::GetDebugStatus() const -> std::string
    {
        std::ostringstream oss;

        oss << fmt::format("-------------------------------------\n");
        oss << fmt::format("stage: {}\n", _stageId);
        oss << fmt::format("entities: {}\n", [this]() -> std::string
            {
                std::ostringstream oss2;

                for (const auto& [type, entities] : _entities)
                {
                    oss2 << fmt::format("{{ type: {}, size: {} }}, ", ToString(type), std::ssize(entities));
                }

                std::string result = oss2.str();
                if (std::ssize(result) > 1)
                {
                    result.pop_back();
                    result.pop_back();
                }

                return result;

            }());

        oss << fmt::format("-------------------------------------\n");

        return oss.str();
    }

    void SceneObjectSystem::HandlePlayerAllState(const ZoneMessage& message)
    {
        GamePlayer& player = message.player;

        player.Defer(GamePlayerMessageCreator::CreateAllState(player));
        player.Defer(GamePlayerMessageCreator::CreateQuestAllState(player));

        if (player.IsDead())
        {
            player.Defer(SceneObjectPacketCreator::CreateState(player, GameEntityState{ .type = GameEntityStateType::Dying }));
        }

        player.FlushDeferred();
    }

    void SceneObjectSystem::HandleRequestItemStructure(const ZoneMessage& message)
    {
        (void)message;
    }

    void SceneObjectSystem::HandleNPCDirectionSet(const ZoneMessage& message)
    {
        SlPacketReader& reader = message.reader;

        const auto& target = FindEntity(GameEntityType::NPC, message.targetId);
        if (!target)
        {
            return;
        }

        const GameNPC* npc = target->Cast<GameNPC>();
        assert(npc);

        if (const int32_t itemId = ItemMixPropItemMapping::FindItemId(npc->GetUnk1(), npc->GetUnk2());
            itemId != 0)
        {
            return;
        }

        const float x = reader.Read<float>();
        const float y = reader.Read<float>();

        const float newYaw = std::atan2f(y, x) * (180.f / static_cast<float>(std::numbers::pi));

        ClientMovement movement = npc->GetComponent<SceneObjectComponent>().GetMovement();
        movement.yaw = newYaw;

        Get<EntityViewRangeSystem>().Broadcast(*npc, ZonePacketS2CCreator::CreateObjectMove(*npc, movement), false);
    }

    void SceneObjectSystem::HandleDirectionSet(const ZoneRequest& message)
    {
        SlPacketReader& reader = message.reader;
        GamePlayer& player = message.player;

        const float yaw = reader.Read<float>();

        SceneObjectComponent& sceneObjectComponent = player.GetSceneObjectComponent();
        sceneObjectComponent.SetYaw(yaw);
        sceneObjectComponent.SetMovementTypeBitMask(0);

        constexpr bool includeSelf = false;
        Get<EntityViewRangeSystem>().Broadcast(player, ZonePacketS2CCreator::CreateObjectMove(player), includeSelf);
    }
}
