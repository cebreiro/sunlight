#include "entity_view_range_system.h"

#include <boost/container/static_vector.hpp>

#include "sl/data/map/map_stage.h"
#include "sl/data/map/map_stage_terrain.h"
#include "sl/emulator/game/game_constant.h"
#include "sl/emulator/game/component/scene_object_component.h"
#include "sl/emulator/game/contants/sector/game_spatial_id.h"
#include "sl/emulator/game/contants/sector/game_spatial_mbr.h"
#include "sl/emulator/game/contants/sector/game_spatial_sector.h"
#include "sl/emulator/game/entity/game_entity.h"
#include "sl/emulator/game/entity/game_item.h"
#include "sl/emulator/game/entity/game_player.h"
#include "sl/emulator/game/message/creator/scene_object_message_creator.h"
#include "sl/emulator/server/packet/creator/zone_packet_s2c_creator.h"

namespace sunlight
{
    EntityViewRangeSystem::EntityViewRangeSystem(const ServiceLocator& serviceLocator, const MapStage& stageData)
        : _serviceLocator(serviceLocator)
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

    EntityViewRangeSystem::~EntityViewRangeSystem()
    {
    }

    void EntityViewRangeSystem::InitializeSubSystem(Stage& stage)
    {
        (void)stage;
    }

    bool EntityViewRangeSystem::Subscribe(Stage& stage)
    {
        (void)stage;

        return true;
    }

    auto EntityViewRangeSystem::GetName() const -> std::string_view
    {
        return "entity_view_range_system";
    }

    auto EntityViewRangeSystem::GetClassId() const -> game_system_id_type
    {
        return GameSystem::GetClassId<EntityViewRangeSystem>();
    }

    void EntityViewRangeSystem::Add(GameEntity& entity)
    {
        assert(!_entitySectorIndex.contains(entity.GetId()));
        assert(entity.HasComponent<SceneObjectComponent>());

        GameSpatialSector& sector = GetSector(entity.GetComponent<SceneObjectComponent>().GetPosition());
        sector.AddEntity(entity);

        _entitySectorIndex[entity.GetId()] = &sector;
    }

    void EntityViewRangeSystem::Remove(GameEntity& entity)
    {
        assert(_entitySectorIndex.contains(entity.GetId()));

        const auto iter = _entitySectorIndex.find(entity.GetId());
        if (iter == _entitySectorIndex.end())
        {
            assert(false);

            return;
        }

        iter->second->RemoveEntity(entity);
        _entitySectorIndex.erase(iter);
    }

    void EntityViewRangeSystem::UpdateViewRange(GameEntity& entity, const Eigen::Vector2f& newPosition)
    {
        const auto iter = _entitySectorIndex.find(entity.GetId());
        if (iter == _entitySectorIndex.end())
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

    void EntityViewRangeSystem::VisitEntity(const Eigen::Vector2f& position,
        const std::function<void(GameEntity&)>& function)
    {
        GameSpatialSector& sector = GetSector(position);

        auto range = sector.GetEntities();
        if (range.begin() == range.end())
        {
            return;
        }

        for (GameEntity& entity : range)
        {
            function(entity);
        }
    }

    void EntityViewRangeSystem::VisitEntity(const GameEntity& centerEntity,
        const std::function<void(GameEntity&)>& function)
    {
        const auto iter = _entitySectorIndex.find(centerEntity.GetId());
        if (iter == _entitySectorIndex.end())
        {
            assert(false);

            return;
        }

        GameSpatialSector& sector = *iter->second;

        auto range = sector.GetEntities();
        if (range.begin() == range.end())
        {
            return;
        }

        for (GameEntity& entity : range)
        {
            function(entity);
        }
    }

    void EntityViewRangeSystem::VisitPlayer(const Eigen::Vector2f& position,
        const std::function<void(GamePlayer&)>& function)
    {
        GameSpatialSector& sector = GetSector(position);

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

    void EntityViewRangeSystem::VisitPlayer(const GameEntity& centerEntity,
        const std::function<void(GamePlayer&)>& function)
    {
        const auto iter = _entitySectorIndex.find(centerEntity.GetId());
        if (iter == _entitySectorIndex.end())
        {
            assert(false);

            return;
        }

        GameSpatialSector& sector = *iter->second;

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

    void EntityViewRangeSystem::Broadcast(const Eigen::Vector2f& position, const Buffer& buffer)
    {
        GameSpatialSector& sector = GetSector(position);

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

    void EntityViewRangeSystem::Broadcast(const GameEntity& centerEntity, const Buffer& buffer, bool includeCenter)
    {
        const game_entity_id_type centerEntityId = centerEntity.GetId();

        const auto iter = _entitySectorIndex.find(centerEntityId);
        if (iter == _entitySectorIndex.end())
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

    auto EntityViewRangeSystem::GetSector(int32_t x, int32_t y) -> GameSpatialSector&
    {
        const int64_t index = CalculateIndex(x, y);
        assert(index >= 0 && index < std::ssize(_sectors));

        return *_sectors[index];
    }

    auto EntityViewRangeSystem::GetSector(int32_t x, int32_t y) const -> const GameSpatialSector&
    {
        const int64_t index = CalculateIndex(x, y);
        assert(index >= 0 && index < std::ssize(_sectors));

        return *_sectors[index];
    }

    auto EntityViewRangeSystem::GetSector(const Eigen::Vector2f& position) -> GameSpatialSector&
    {
        return GetSector(CalculateXIndex(position.x()), CalculateXIndex(position.y()));
    }

    auto EntityViewRangeSystem::GetSector(const Eigen::Vector2f& position) const -> const GameSpatialSector&
    {
        return GetSector(CalculateXIndex(position.x()), CalculateXIndex(position.y()));
    }

    auto EntityViewRangeSystem::CalculateXIndex(float x) const -> int32_t
    {
        const int32_t intValue = static_cast<int32_t>(x);
        const int32_t clamped = std::clamp(intValue, 0, _width);

        return clamped / cell_size;
    }

    auto EntityViewRangeSystem::CalculateYIndex(float y) const -> int32_t
    {
        const int32_t intValue = static_cast<int32_t>(y);
        const int32_t clamped = std::clamp(intValue, 0, _height);

        return clamped / cell_size;
    }

    auto EntityViewRangeSystem::CalculateIndex(int32_t x, int32_t y) const -> int64_t
    {
        assert(x >= 0 && x < _xSize);
        assert(y >= 0 && y < _ySize);

        return x + (y * _xSize);
    }
}
