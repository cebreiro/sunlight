#include "scene_object_system.h"

#include "sl/emulator/game/component/player_appearance_component.h"
#include "sl/emulator/game/component/player_group_component.h"
#include "sl/emulator/game/component/player_npc_script_component.h"
#include "sl/emulator/game/component/scene_object_component.h"
#include "sl/emulator/game/contents/group/item_mix_prop_item_mapping.h"
#include "sl/emulator/game/contents/state/game_entity_state.h"
#include "sl/emulator/game/entity/game_item.h"
#include "sl/emulator/game/entity/game_npc.h"
#include "sl/emulator/game/entity/game_player.h"
#include "sl/emulator/game/entity/game_stored_item.h"
#include "sl/emulator/game/message/zone_message.h"
#include "sl/emulator/game/message/creator/game_player_message_creator.h"
#include "sl/emulator/game/message/creator/scene_object_message_creator.h"
#include "sl/emulator/game/system/entity_view_range_system.h"
#include "sl/emulator/game/system/entity_movement_system.h"
#include "sl/emulator/game/system/player_appearance_system.h"
#include "sl/emulator/game/system/player_quest_system.h"
#include "sl/emulator/game/system/player_stat_system.h"
#include "sl/emulator/game/zone/stage.h"
#include "sl/emulator/game/zone/service/game_entity_id_publisher.h"
#include "sl/emulator/server/packet/creator/zone_packet_s2c_creator.h"

namespace sunlight
{
    const std::shared_ptr<GameEntity> SceneObjectSystem::null_shared_entity;

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
        Add(stage.Get<PlayerQuestSystem>());
    }

    bool SceneObjectSystem::Subscribe(Stage& stage)
    {
        if (!stage.AddSubscriber(ZoneMessageType::REQUEST_ALL_STATE,
            std::bind_front(&SceneObjectSystem::HandlePlayerAllState, this)))
        {
            return false;
        }

        if (!stage.AddSubscriber(ZoneMessageType::LOCAL_ACTIVATED,
            std::bind_front(&SceneObjectSystem::HandlePlayerActivate, this)))
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

    void SceneObjectSystem::SpawnPlayer(SharedPtrNotNull<GamePlayer> player, StageEnterType enterType)
    {
        _entities[player->GetType()][player->GetId()] = player;

        Get<PlayerStatSystem>().OnInitialize(*player);
        Get<PlayerQuestSystem>().OnInitialize(*player);
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

    void SceneObjectSystem::SpawnItem(SharedPtrNotNull<GameItem> item, Eigen::Vector2f originPos, Eigen::Vector2f destPos)
    {
        if (!item->HasComponent<SceneObjectComponent>())
        {
            item->AddComponent(std::make_unique<SceneObjectComponent>());
        }

        _entities[item->GetType()][item->GetId()] = item;

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
    }

    void SceneObjectSystem::SpawnItem(SharedPtrNotNull<GameStoredItem> item)
    {
        assert(item->HasComponent<SceneObjectComponent>());

        _entities[item->GetType()][item->GetId()] = item;

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

        switch (exitType)
        {
        case StageExitType::Logout:
        {
            Get<EntityViewRangeSystem>().Broadcast(player,
                SceneObjectPacketCreator::CreateState(player, GameEntityState{ .type = GameEntityStateType::Leaving, }), false);
        }
        break;
        case StageExitType::ZoneChange:
        {
            Get<EntityViewRangeSystem>().Broadcast(player,
                SceneObjectPacketCreator::CreateState(player, GameEntityState{ .type = GameEntityStateType::ChangingZone, }), true);
        }
        break;
        case StageExitType::StageChange:
        {
            Get<EntityViewRangeSystem>().Broadcast(player,
                SceneObjectPacketCreator::CreateState(player, GameEntityState{ .type = GameEntityStateType::ChangingRoom, }), true);
        }
        break;
        }

        Get<EntityViewRangeSystem>().Remove(player);
        Get<EntityMovementSystem>().Remove(player.GetId());

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

        iter1->second.erase(iter2);

        return true;
    }

    auto SceneObjectSystem::FindEntity(GameEntityType type, game_entity_id_type id) -> const std::shared_ptr<GameEntity>&
    {
        const auto iter1 = _entities.find(type);
        if (iter1 == _entities.end())
        {
            return null_shared_entity;
        }

        const auto iter2 = iter1->second.find(id);
        if (iter2 == iter1->second.end())
        {
            return null_shared_entity;
        }

        return iter2->second;
    }

    auto SceneObjectSystem::FindEntity(GameEntityType type, game_entity_id_type id) const -> const std::shared_ptr<GameEntity>&
    {
        const auto iter1 = _entities.find(type);
        if (iter1 == _entities.end())
        {
            return null_shared_entity;
        }

        const auto iter2 = iter1->second.find(id);
        if (iter2 == iter1->second.end())
        {
            return null_shared_entity;
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
        message.player.Send(ZonePacketS2CCreator::CreateObjectVisibleRange(1000.f));
        message.player.Send(GamePlayerMessageCreator::CreateAllState(message.player));
        message.player.Send(GamePlayerMessageCreator::CreateQuestAllState(message.player));
    }

    void SceneObjectSystem::HandlePlayerActivate(const ZoneMessage& message)
    {
        GamePlayer& player = message.player;

        player.SetActive(true);

        Get<PlayerStatSystem>().OnLocalActivate(player);
    }

    void SceneObjectSystem::HandleRequestItemStructure(const ZoneMessage& message)
    {
        (void)message;

        // origin zone message protocol
        // 1) server -> client / object_move, nms_user_info
        // 2) client -> server / request_item_structure (here)
        // 3) server -> client / create_item
        
        // but, this emulator does not respect origin protocol
        // create_item is sent at process (1)
    }

    void SceneObjectSystem::HandleNPCDirectionSet(const ZoneMessage& message)
    {
        SlPacketReader& reader = message.reader;
        GamePlayer& player = message.player;

        PlayerNPCScriptComponent& scriptComponent = player.GetNPCScriptComponent();
        if (!scriptComponent.HasTargetNPC())
        {
            return;
        }

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
        const float prevYaw = scriptComponent.GetNPCYaw();

        if (std::abs(newYaw - prevYaw) < 1.f)
        {
            return;
        }

        scriptComponent.SetNPCYaw(newYaw);

        ClientMovement movement = npc->GetComponent<SceneObjectComponent>().GetMovement();
        movement.yaw = newYaw;

        Get<EntityViewRangeSystem>().Broadcast(*npc, ZonePacketS2CCreator::CreateObjectMove(*npc, movement), false);
    }
}
