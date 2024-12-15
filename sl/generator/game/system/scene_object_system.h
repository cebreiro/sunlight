#pragma once
#include <boost/unordered/unordered_flat_map.hpp>
#include "sl/generator/game/entity/game_entity_id_type.h"
#include "sl/generator/game/entity/game_entity_type.h"
#include "sl/generator/game/entity/game_player.h"
#include "sl/generator/game/system/game_system.h"
#include "sl/generator/game/zone/stage_enter_type.h"
#include "sl/generator/game/zone/stage_exit_type.h"

namespace sunlight
{
    struct MapStage;
    struct ZoneRequest;
    struct ZoneMessage;

    class GameEntity;
    class GamePlayer;
    class GameNPC;
    class GameItem;
    class GameStoredItem;
    class GameMonster;

    class GameSpatialSector;
    class GameSpatialCell;
}

namespace sunlight
{
    class SceneObjectSystem final : public GameSystem
    {
    public:
        SceneObjectSystem(const ServiceLocator& serviceLocator, int32_t stageId);
        ~SceneObjectSystem();

        void InitializeSubSystem(Stage& stage) override;
        bool Subscribe(Stage& stage) override;
        auto GetName() const -> std::string_view override;
        auto GetClassId() const -> game_system_id_type override;

    public:
        bool Contains(GameEntityType type, game_entity_id_type id) const;

    public:
        void SpawnPlayer(SharedPtrNotNull<GamePlayer> player, StageEnterType enterType);
        bool SpawnNPC(SharedPtrNotNull<GameNPC> npc);
        void SpawnItem(SharedPtrNotNull<GameItem> item, Eigen::Vector2f originPos, Eigen::Vector2f destPos, std::optional<std::chrono::seconds> keepDuration);
        void SpawnItem(SharedPtrNotNull<GameStoredItem> item);
        void SpawnMonster(SharedPtrNotNull<GameMonster> monster, Eigen::Vector2f pos, float yaw);

        bool DespawnPlayer(game_entity_id_type id, StageExitType exitType);
        void RemoveItem(game_entity_id_type id);
        bool RemoveStoredItem(game_entity_id_type id);
        bool RemoveMonster(game_entity_id_type id);

    public:
        auto FindEntity(game_entity_id_type id) -> GameEntity*;
        auto FindEntity(game_entity_id_type id) const -> const GameEntity*;
        auto FindEntity(GameEntityType type, game_entity_id_type id) -> GameEntity*;
        auto FindEntity(GameEntityType type, game_entity_id_type id) const -> const GameEntity*;
        auto FindEntityShared(GameEntityType type, game_entity_id_type id) -> std::shared_ptr<GameEntity>;
        auto FindEntityShared(GameEntityType type, game_entity_id_type id) const ->std::shared_ptr<const GameEntity>;

        auto GetDebugStatus() const -> std::string;

    private:
        void HandlePlayerAllState(const ZoneMessage& message);
        void HandleRequestItemStructure(const ZoneMessage& message);
        void HandleNPCDirectionSet(const ZoneMessage& message);
        void HandleDirectionSet(const ZoneRequest& message);

    private:
        const ServiceLocator& _serviceLocator;
        int32_t _stageId;

        std::unordered_map<GameEntityType,
            boost::unordered::unordered_flat_map<game_entity_id_type, SharedPtrNotNull<GameEntity>>> _entities;
        boost::unordered::unordered_flat_map<game_entity_id_type, PtrNotNull<GameEntity>> _entityIdIndex;
    };
}
