#pragma once
#include <boost/unordered/unordered_flat_map.hpp>

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
        explicit SceneObjectSystem(const ServiceLocator& serviceLocator);
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
        auto FindEntity(GameEntityType type, game_entity_id_type id) -> const std::shared_ptr<GameEntity>&;
        auto FindEntity(GameEntityType type, game_entity_id_type id) const -> const std::shared_ptr<GameEntity>&;

    private:
        void HandlePlayerAllState(const ZoneMessage& message);
        void HandlePlayerActivate(const ZoneMessage& message);

        void HandleRequestItemStructure(const ZoneMessage& message);

    private:
        const ServiceLocator& _serviceLocator;

        std::unordered_map<GameEntityType,
            boost::unordered::unordered_flat_map<game_entity_id_type, SharedPtrNotNull<GameEntity>>> _entities;

        static const std::shared_ptr<GameEntity> null_shared_entity;
    };
}
