#pragma once
#include <boost/unordered/unordered_flat_map.hpp>

#include "sl/emulator/game/game_constant.h"
#include "sl/emulator/game/entity/game_entity_id_type.h"
#include "sl/emulator/game/entity/game_entity_type.h"
#include "sl/emulator/game/entity/game_player.h"
#include "sl/emulator/game/system/game_system.h"

namespace sunlight
{
    struct MapStage;
    struct ZoneMessage;

    class GameEntity;
    class GamePlayer;
    class GameItem;

    class GameSpatialSector;
    class GameSpatialCell;
}

namespace sunlight
{
    class SceneObjectSystem final : public GameSystem
    {
    public:
        SceneObjectSystem(const ServiceLocator& serviceLocator, const MapStage& stageData);
        ~SceneObjectSystem();

        void InitializeSubSystem(Stage& stage) override;
        bool Subscribe(Stage& stage) override;
        auto GetName() const -> std::string_view override;
        auto GetClassId() const -> game_system_id_type override;

    public:
        void SpawnPlayer(SharedPtrNotNull<GamePlayer> player);
        void SpawnItem(SharedPtrNotNull<GameItem> item, Eigen::Vector2f originPos, Eigen::Vector2f destPos);

        void RemoveItem(game_entity_id_type id);

    public:
        void UpdateViewRange(GameEntity& entity, const Eigen::Vector2f& newPosition);

        void Broadcast(game_entity_id_type centerEntityId, const Buffer& buffer, bool includeCenter);
        void Broadcast(const Eigen::Vector2f& position, const Buffer& buffer);
        void Broadcast(GameSpatialSector& sector, const Buffer& buffer);

        void Visit(GameSpatialSector& sector, const std::function<void(GamePlayer&)>& function);

    public:
        auto FindEntity(GameEntityType type, game_entity_id_type id) -> const std::shared_ptr<GameEntity>&;
        auto FindEntity(GameEntityType type, game_entity_id_type id) const -> const std::shared_ptr<GameEntity>&;

    private:
        void HandlePlayerAllState(const ZoneMessage& message);
        void HandlePlayerActivate(const ZoneMessage& message);

        void HandleRequestItemStructure(const ZoneMessage& message);

    private:
        auto GetSector(int32_t x, int32_t y) -> GameSpatialSector&;
        auto GetSector(int32_t x, int32_t y) const -> const GameSpatialSector&;
        auto GetSector(const Eigen::Vector2f& position) -> GameSpatialSector&;
        auto GetSector(const Eigen::Vector2f& position) const -> const GameSpatialSector&;

        auto CalculateXIndex(float x) const -> int32_t;
        auto CalculateYIndex(float y) const -> int32_t;
        auto CalculateIndex(int32_t x, int32_t y) const -> int64_t;

    private:
        const ServiceLocator& _serviceLocator;

        int32_t _id = 0;
        int32_t _width = 0;
        int32_t _height = 0;
        int32_t _xSize = 0;
        int32_t _ySize = 0;

        static constexpr int32_t cell_size = 1000;

        std::vector<UniquePtrNotNull<GameSpatialSector>> _sectors;
        std::vector<UniquePtrNotNull<GameSpatialCell>> _cells;

        std::unordered_map<game_entity_id_type, PtrNotNull<GameSpatialSector>> _entityViewRange;

        std::unordered_map<GameEntityType,
            boost::unordered::unordered_flat_map<game_entity_id_type, SharedPtrNotNull<GameEntity>>> _entities;

        static const std::shared_ptr<GameEntity> null_shared_entity;
    };
}
