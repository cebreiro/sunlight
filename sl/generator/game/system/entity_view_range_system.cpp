#include "entity_view_range_system.h"

#include <boost/container/static_vector.hpp>

#include "sl/data/map/map_stage.h"
#include "sl/data/map/map_stage_room.h"
#include "sl/data/map/map_stage_terrain.h"
#include "sl/generator/game/game_constant.h"
#include "sl/generator/game/component/player_appearance_component.h"
#include "sl/generator/game/component/player_group_component.h"
#include "sl/generator/game/component/scene_object_component.h"
#include "sl/generator/game/contents/sector/game_spatial_id.h"
#include "sl/generator/game/contents/sector/game_spatial_mbr.h"
#include "sl/generator/game/contents/sector/game_spatial_sector.h"
#include "sl/generator/game/contents/state/game_entity_state.h"
#include "sl/generator/game/entity/game_item.h"
#include "sl/generator/game/entity/game_monster.h"
#include "sl/generator/game/entity/game_npc.h"
#include "sl/generator/game/entity/game_player.h"
#include "sl/generator/game/entity/game_stored_item.h"
#include "sl/generator/game/message/creator/game_player_message_creator.h"
#include "sl/generator/game/message/creator/scene_object_message_creator.h"
#include "sl/generator/server/packet/creator/zone_packet_s2c_creator.h"

namespace sunlight
{
    EntityViewRangeSystem::EntityViewRangeSystem(const ServiceLocator& serviceLocator, const MapStage& stageData)
        : _serviceLocator(serviceLocator)
    {
        if (stageData.terrain)
        {
            _width = stageData.terrain->width * static_cast<int32_t>(GameConstant::STAGE_TERRAIN_BLOCK_SIZE);
            _height = stageData.terrain->height * static_cast<int32_t>(GameConstant::STAGE_TERRAIN_BLOCK_SIZE);
        }

        if (stageData.room)
        {
            _width = stageData.room->width * static_cast<int32_t>(GameConstant::STAGE_TERRAIN_BLOCK_SIZE);
            _height = stageData.room->height * static_cast<int32_t>(GameConstant::STAGE_TERRAIN_BLOCK_SIZE);
        }

        assert(_width > 0);
        assert(_height > 0);

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
            const float minY = static_cast<float>(y * cell_size);
            const float maxY = static_cast<float>(minY) + cell_size;

            for (int32_t x = 0; x < _xSize; ++x)
            {
                const float minX = static_cast<float>(x * cell_size);
                const float maxX = static_cast<float>(minX + cell_size);

                const GameSpatialMBR mbr(Eigen::Vector2f(minX, minY), Eigen::Vector2f(maxX, maxY));

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

    bool EntityViewRangeSystem::IsAdjacent(const GameEntity& entity, const GameEntity& other) const
    {
        assert(entity.HasComponent<SceneObjectComponent>());
        assert(other.HasComponent<SceneObjectComponent>());

        const game_spatial_cell_id_type id1 = GetSector(entity).GetCenter().GetId();
        const game_spatial_cell_id_type id2 = GetSector(other).GetCenter().GetId();

        if (std::abs(id1.GetX() - id2.GetX()) > 1)
        {
            return false;
        }

        if (std::abs(id1.GetY() - id2.GetY()) > 1)
        {
            return false;
        }

        return true;
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
                        if (GamePlayer* targetPlayer = target.Cast<GamePlayer>(); targetPlayer)
                        {
                            targetPlayer->Defer(ZonePacketS2CCreator::CreateObjectMove(entity));

                            switch (entity.GetType())
                            {
                            case GameEntityType::Player:
                            {
                                assert(player && player == &entity);

                                targetPlayer->Defer(SceneObjectPacketCreator::CreateInformation(*player, false));
                                targetPlayer->Defer(GamePlayerMessageCreator::CreateRemotePlayerState(*player));

                                if (const PlayerAppearanceComponent& appearanceComponent = player->GetAppearanceComponent();
                                    appearanceComponent.GetHatModelId() != 0)
                                {
                                    targetPlayer->Defer(GamePlayerMessageCreator::CreatePlayerHairColorChange(*player, appearanceComponent.GetHairColor()));
                                }
                            }
                            break;
                            case GameEntityType::NPC:
                            {
                                const GameNPC& npc = *entity.Cast<GameNPC>();

                                targetPlayer->Defer(SceneObjectPacketCreator::CreateInformation(npc));
                            }
                            break;
                            case GameEntityType::Enemy:
                            {
                                const GameMonster& monster = *entity.Cast<GameMonster>();

                                constexpr bool spawnEffect = false;
                                targetPlayer->Defer(SceneObjectPacketCreator::CreateInformation(monster, spawnEffect));
                            }
                            break;
                            }

                            targetPlayer->FlushDeferred();
                        }

                        if (player)
                        {
                            player->Defer(ZonePacketS2CCreator::CreateObjectMove(target));

                            switch (target.GetType())
                            {
                            case GameEntityType::Player:
                            {
                                GamePlayer& targetPlayer = *target.Cast<GamePlayer>();

                                player->Defer(SceneObjectPacketCreator::CreateInformation(targetPlayer, false));
                                player->Defer(GamePlayerMessageCreator::CreateRemotePlayerState(targetPlayer));

                                if (const PlayerAppearanceComponent& appearanceComponent = targetPlayer.GetAppearanceComponent();
                                    appearanceComponent.GetHatModelId() != 0)
                                {
                                    player->Defer(GamePlayerMessageCreator::CreatePlayerHairColorChange(targetPlayer, appearanceComponent.GetHairColor()));
                                }

                                if (PlayerGroupComponent& otherGroupComponent = targetPlayer.GetGroupComponent();
                                    otherGroupComponent.HasGroup())
                                {
                                    player->Defer(GamePlayerMessageCreator::CreatePlayerStateProposition(targetPlayer, otherGroupComponent.GetGroupState()));

                                    if (otherGroupComponent.GetGroupType() == GameGroupType::StreetVendor)
                                    {
                                        player->Defer(SceneObjectPacketCreator::CreateState(targetPlayer, GameEntityState{ .type = GameEntityStateType::StreetVendor }));
                                    }
                                }
                            }
                            break;
                            case GameEntityType::NPC:
                            {
                                const GameNPC& npc = *target.Cast<GameNPC>();

                                player->Defer(SceneObjectPacketCreator::CreateInformation(npc));
                            }
                            break;
                            case GameEntityType::Item:
                            {
                                const GameItem& item = *target.Cast<GameItem>();

                                player->Defer(SceneObjectPacketCreator::CreateInformation(item));
                                player->Defer(SceneObjectPacketCreator::CreateItemDisplay(item, player->GetCId()));
                            }
                            break;
                            case GameEntityType::ItemChild:
                            {
                                const GameStoredItem& item = *target.Cast<GameStoredItem>();

                                player->Defer(SceneObjectPacketCreator::CreateInformation(item));
                                player->Defer(SceneObjectPacketCreator::CreateItemDisplay(item));
                            }
                            break;
                            case GameEntityType::Enemy:
                            {
                                const GameMonster& monster = *target.Cast<GameMonster>();

                                constexpr bool spawnEffect = false;
                                player->Defer(SceneObjectPacketCreator::CreateInformation(monster, spawnEffect));
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

    void EntityViewRangeSystem::VisitMonster(const GameEntity& centerEntity, const std::function<void(GameMonster&)>& function)
    {
        const auto iter = _entitySectorIndex.find(centerEntity.GetId());
        if (iter == _entitySectorIndex.end())
        {
            assert(false);

            return;
        }

        GameSpatialSector& sector = *iter->second;

        auto range = sector.GetEntities(GameMonster::TYPE);
        if (range.begin() == range.end())
        {
            return;
        }

        for (GameEntity& entity : range)
        {
            GameMonster* monster = entity.Cast<GameMonster>();
            if (!monster)
            {
                assert(false);

                continue;
            }

            function(*monster);
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

    auto EntityViewRangeSystem::GetSector(const GameEntity& entity) -> GameSpatialSector&
    {
        const auto iter = _entitySectorIndex.find(entity.GetId());
        if (iter == _entitySectorIndex.end())
        {
            assert(false);

            return *_sectors[0];
        }

        return *iter->second;
    }

    auto EntityViewRangeSystem::GetSector(const GameEntity& entity) const -> const GameSpatialSector&
    {
        const auto iter = _entitySectorIndex.find(entity.GetId());
        if (iter == _entitySectorIndex.end())
        {
            assert(false);

            return *_sectors[0];
        }

        return *iter->second;
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
        return GetSector(CalculateXIndex(position.x()), CalculateYIndex(position.y()));
    }

    auto EntityViewRangeSystem::GetSector(const Eigen::Vector2f& position) const -> const GameSpatialSector&
    {
        return GetSector(CalculateXIndex(position.x()), CalculateYIndex(position.y()));
    }

    auto EntityViewRangeSystem::CalculateXIndex(float x) const -> int32_t
    {
        const int32_t intValue = static_cast<int32_t>(x);
        const int32_t clamped = std::clamp(intValue, 0, _width - 1);

        return clamped / cell_size;
    }

    auto EntityViewRangeSystem::CalculateYIndex(float y) const -> int32_t
    {
        const int32_t intValue = static_cast<int32_t>(y);
        const int32_t clamped = std::clamp(intValue, 0, _height - 1);

        return clamped / cell_size;
    }

    auto EntityViewRangeSystem::CalculateIndex(int32_t x, int32_t y) const -> int64_t
    {
        assert(x >= 0 && x < _xSize);
        assert(y >= 0 && y < _ySize);

        return x + (y * _xSize);
    }
}
