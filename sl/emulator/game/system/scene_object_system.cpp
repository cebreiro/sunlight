#include "scene_object_system.h"

#include "sl/data/map/map_stage.h"
#include "sl/data/map/map_stage_terrain.h"
#include "sl/emulator/game/game_constant.h"
#include "sl/emulator/game/component/scene_object_component.h"
#include "sl/emulator/game/component/item_position_component.h"
#include "sl/emulator/game/contants/sector/game_spatial_id.h"
#include "sl/emulator/game/contants/sector/game_spatial_sector.h"
#include "sl/emulator/game/entity/game_item.h"
#include "sl/emulator/game/entity/game_player.h"
#include "sl/emulator/game/message/zone_message.h"
#include "sl/emulator/game/message/creator/game_player_message_creator.h"
#include "sl/emulator/game/message/creator/scene_object_message_creator.h"
#include "sl/emulator/game/system/player_stat_system.h"
#include "sl/emulator/game/zone/stage.h"
#include "sl/emulator/game/zone/service/game_entity_id_publisher.h"
#include "sl/emulator/server/packet/creator/zone_packet_s2c_creator.h"

namespace sunlight
{
    const std::shared_ptr<GameEntity> SceneObjectSystem::null_shared_entity;

    SceneObjectSystem::SceneObjectSystem(const ServiceLocator& serviceLocator, const MapStage& stageData)
        : _serviceLocator(serviceLocator)
        , _id(stageData.id)
    {
        if (stageData.terrain)
        {
            _width = stageData.terrain->width * static_cast<int32_t>(GameConstant::STAGE_TERRAIN_BLOCK_SIZE);
            _height = stageData.terrain->height * static_cast<int32_t>(GameConstant::STAGE_TERRAIN_BLOCK_SIZE);

            _xSize = (_width / cell_size) + 1;
            _ySize = (_height / cell_size) + 1;

            const int32_t size = _xSize * _ySize;

            _cells.reserve(size);
            _sectors.reserve(size);

            [[maybe_unused]] const size_t cellsCapacity = _cells.capacity();
            [[maybe_unused]] const size_t sectorsCapacity = _sectors.capacity();

            for (int32_t y = 0; y < _ySize; ++y)
            {
                for (int32_t x = 0; x < _xSize; ++x)
                {
                    _sectors.emplace_back(std::make_unique<GameSpatialSector>(game_spatial_sector_id_type(x, y)));
                }
            }

            auto getAdjacentSectors = [this](int32_t x, int32_t y) -> boost::container::static_vector<PtrNotNull<GameSpatialSector>, 9>
                {
                    boost::container::static_vector<PtrNotNull<GameSpatialSector>, 9> result;

                    for (int32_t j = y - 1; j <= y + 1; ++j)
                    {
                        if (j < 0 || j >= _ySize)
                        {
                            continue;
                        }

                        for (int32_t i = x - 1; i <= x + 1; ++i)
                        {
                            if (i < 0 || i >= _xSize)
                            {
                                continue;
                            }

                            GameSpatialSector& sector = GetSector(i, j);

                            result.push_back(&sector);
                        }
                    }

                    return result;
                };

            for (int32_t y = 0; y < _ySize; ++y)
            {
                const double minY = y * cell_size;
                const double maxY = minY + cell_size;

                for (int32_t x = 0; x < _xSize; ++x)
                {
                    const double minX = x * cell_size;
                    const double maxX = minX + cell_size;

                    const GameSpatialMBR mbr(Eigen::Vector2d(minX, minY), Eigen::Vector2d(maxX, maxY));

                    GameSpatialCell& cell = *_cells.emplace_back(std::make_unique<GameSpatialCell>(game_spatial_cell_id_type(x, y), mbr));

                    for (PtrNotNull<GameSpatialSector> sector : getAdjacentSectors(x, y))
                    {
                        sector->AddCell(&cell);
                    }
                }
            }

            assert(cellsCapacity == _cells.capacity());
            assert(cellsCapacity == _cells.size());
            assert(sectorsCapacity == _sectors.capacity());
            assert(sectorsCapacity == _sectors.size());
        }
    }

    SceneObjectSystem::~SceneObjectSystem()
    {
    }

    void SceneObjectSystem::InitializeSubSystem(Stage& stage)
    {
        Add(stage.Get<PlayerStatSystem>());
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

    void SceneObjectSystem::SpawnPlayer(SharedPtrNotNull<GamePlayer> player)
    {
        assert(!_entityViewRange.contains(player->GetId()));

        _entities[player->GetType()][player->GetId()] = player;

        Get<PlayerStatSystem>().OnInitialize(*player);

        SceneObjectComponent& sceneObjectComponent = player->GetSceneObjectComponent();
        sceneObjectComponent.SetId(_serviceLocator.Get<GameEntityIdPublisher>().PublishSceneObjectId(player->GetType()));

        GameSpatialSector& sector = GetSector(sceneObjectComponent.GetPosition());
        _entityViewRange[player->GetId()] = &sector;

        // TODO: do sync

        sector.AddEntity(*player);
    }

    void SceneObjectSystem::SpawnItem(SharedPtrNotNull<GameItem> item, Eigen::Vector2f originPos, Eigen::Vector2f destPos)
    {
        assert(!_entityViewRange.contains(item->GetId()));

        if (!item->HasComponent<SceneObjectComponent>())
        {
            item->AddComponent(std::make_unique<SceneObjectComponent>());
        }

        _entities[item->GetType()][item->GetId()] = item;

        SceneObjectComponent& sceneObjectComponent = item->GetComponent<SceneObjectComponent>();
        sceneObjectComponent.SetId(_serviceLocator.Get<GameEntityIdPublisher>().PublishSceneObjectId(item->GetType()));
        sceneObjectComponent.SetPosition(destPos);
        sceneObjectComponent.SetDestPosition(destPos);

        GameSpatialSector& sector = GetSector(sceneObjectComponent.GetPosition());
        _entityViewRange[item->GetId()] = &sector;

        Visit(sector, [&](GamePlayer& player)
            {
                player.Defer(ZonePacketS2CCreator::CreateObjectMove(*item));
                player.Defer(SceneObjectPacketCreator::CreateInformation(*item));
                player.Defer(SceneObjectPacketCreator::CreateItemSpawn(*item, player.GetCId(), originPos));

                player.FlushDeferred();
            });

        sector.AddEntity(*item);
    }

    void SceneObjectSystem::RemoveItem(game_entity_id_type id)
    {
        const auto iter1 = _entityViewRange.find(id);
        if (iter1 == _entityViewRange.end())
        {
            return;
        }

        const auto iter2 = _entities.find(GameEntityType::Item);
        if (iter2 == _entities.end())
        {
            assert(false);

            return;
        }

        const auto iter3 = iter2->second.find(id);
        if (iter3 == iter2->second.end())
        {
            assert(false);

            return;
        }

        GameEntity& entity = *iter3->second;
        GameSpatialSector& sector = *iter1->second;

        Broadcast(sector, ZonePacketS2CCreator::CreateObjectLeave(entity));

        sector.RemoveEntity(entity);

        iter2->second.erase(id);
        _entityViewRange.erase(iter1);
    }

    void SceneObjectSystem::UpdateViewRange(GameEntity& entity, const Eigen::Vector2f& newPosition)
    {
        const auto iter = _entityViewRange.find(entity.GetId());
        if (iter == _entityViewRange.end())
        {
            assert(false);

            return;
        }

        GameSpatialSector& oldSector = *iter->second;
        assert(oldSector.GetCenter().Contains(entity.GetType(), entity.GetId()));

        GameSpatialSector& newSector = GetSector(newPosition);
        if (&oldSector == &newSector)
        {
            return;
        }

        GamePlayer* player = entity.Cast<GamePlayer>();
        oldSector.RemoveEntity(entity);

        if (GameSpatialSector::Subset outs = (oldSector - newSector); !outs.Empty())
        {
            for (GameSpatialCell& cell : outs.GetCells())
            {
                if (player || !cell.Empty(GameEntityType::Player))
                {
                    for (GameEntity& target : cell.GetEntities())
                    {
                        if (GamePlayer* targetPlayer = target.Cast<GamePlayer>())
                        {
                            targetPlayer->Send(ZonePacketS2CCreator::CreateObjectLeave(entity));
                        }

                        if (player)
                        {
                            player->Defer(ZonePacketS2CCreator::CreateObjectLeave(target));
                        }
                    }
                }
            }
        }

        if (GameSpatialSector::Subset ins = (newSector - oldSector); !ins.Empty())
        {
            for (GameSpatialCell& in : ins.GetCells())
            {
                if (player || !in.Empty(GameEntityType::Player))
                {
                    for (GameEntity& target : in.GetEntities())
                    {
                        if (GamePlayer* targetPlayer = target.Cast<GamePlayer>())
                        {
                            targetPlayer->Send(ZonePacketS2CCreator::CreateObjectMove(entity));
                        }

                        if (player)
                        {
                            player->Defer(ZonePacketS2CCreator::CreateObjectMove(target));

                            switch (target.GetType())
                            {
                            case GameEntityType::Player:
                            {
                                assert(false);
                            }
                            break;
                            case GameEntityType::Item:
                            {
                                GameItem* item = target.Cast<GameItem>();
                                assert(item);

                                player->Defer(SceneObjectPacketCreator::CreateInformation(*item));
                                player->Defer(SceneObjectPacketCreator::CreateItemDisplay(*item, player->GetCId()));
                            }
                            break;
                            default:
                                assert(false);
                            }
                        }
                    }
                }
            }
        }

        newSector.AddEntity(entity);
        iter->second = &newSector;

        if (player && player->HasDeferred())
        {
            player->FlushDeferred();
        }
    }

    void SceneObjectSystem::Broadcast(game_entity_id_type centerEntityId, const Buffer& buffer, bool includeCenter)
    {
        const auto iter = _entityViewRange.find(centerEntityId);
        if (iter == _entityViewRange.end())
        {
            assert(false);

            return;
        }

        auto range = iter->second->GetEntities(GameEntityType::Player);
        if (range.begin() == range.end())
        {
            return;
        }

        for (GameEntity& entity : range)
        {
            if (!includeCenter && entity.GetId() == centerEntityId)
            {
                continue;
            }

            GamePlayer* player = entity.Cast<GamePlayer>();
            if (!player)
            {
                assert(false);

                continue;
            }

            player->Send(buffer.DeepCopy());
        }
    }

    void SceneObjectSystem::Broadcast(const Eigen::Vector2f& position, const Buffer& buffer)
    {
        Broadcast(GetSector(position), buffer);
    }

    void SceneObjectSystem::Broadcast(GameSpatialSector& sector, const Buffer& buffer)
    {
        auto range = sector.GetEntities(GameEntityType::Player);
        if (range.begin() == range.end())
        {
            return;
        }

        for (GameEntity& entity : range)
        {
            GamePlayer* player = entity.Cast<GamePlayer>();
            if (!player)
            {
                assert(false);

                continue;
            }

            player->Send(buffer.DeepCopy());
        }
    }

    void SceneObjectSystem::Visit(GameSpatialSector& sector, const std::function<void(GamePlayer&)>& function)
    {
        auto range = sector.GetEntities(GameEntityType::Player);
        if (range.begin() == range.end())
        {
            return;
        }

        for (GameEntity& entity : range)
        {
            GamePlayer* player = entity.Cast<GamePlayer>();
            if (!player)
            {
                assert(false);

                continue;
            }

            function(*player);
        }
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

    void SceneObjectSystem::HandlePlayerAllState(const ZoneMessage& message)
    {
        message.player.Send(GamePlayerMessageCreator::CreateAllState(message.player));
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

    auto SceneObjectSystem::GetSector(int32_t x, int32_t y) -> GameSpatialSector&
    {
        const int64_t index = CalculateIndex(x, y);
        assert(index >= 0 && index < std::ssize(_sectors));

        return *_sectors[index];
    }

    auto SceneObjectSystem::GetSector(int32_t x, int32_t y) const -> const GameSpatialSector&
    {
        const int64_t index = CalculateIndex(x, y);
        assert(index >= 0 && index < std::ssize(_sectors));

        return *_sectors[index];
    }

    auto SceneObjectSystem::GetSector(const Eigen::Vector2f& position) -> GameSpatialSector&
    {
        return GetSector(CalculateXIndex(position.x()), CalculateXIndex(position.y()));
    }

    auto SceneObjectSystem::GetSector(const Eigen::Vector2f& position) const -> const GameSpatialSector&
    {
        return GetSector(CalculateXIndex(position.x()), CalculateXIndex(position.y()));
    }

    auto SceneObjectSystem::CalculateXIndex(float x) const -> int32_t
    {
        const int32_t intValue = static_cast<int32_t>(x);
        const int32_t clamped = std::clamp(intValue, 0, _width);

        return clamped / cell_size;
    }

    auto SceneObjectSystem::CalculateYIndex(float y) const -> int32_t
    {
        const int32_t intValue = static_cast<int32_t>(y);
        const int32_t clamped = std::clamp(intValue, 0, _height);

        return clamped / cell_size;
    }

    auto SceneObjectSystem::CalculateIndex(int32_t x, int32_t y) const -> int64_t
    {
        assert(x >= 0 && x < _xSize);
        assert(y >= 0 && y < _ySize);

        return x + (y * _xSize);
    }
}
