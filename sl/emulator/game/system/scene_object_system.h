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
    class GameNPC;
    class GameItem;

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
        void SpawnPlayer(SharedPtrNotNull<GamePlayer> player);
        bool SpawnNPC(SharedPtrNotNull<GameNPC> npc);
        void SpawnItem(SharedPtrNotNull<GameItem> item, Eigen::Vector2f originPos, Eigen::Vector2f destPos);

        void RemoveItem(game_entity_id_type id);

    public:
        auto FindPlayerByCid(int64_t cid) -> GamePlayer*;

        auto FindEntity(GameEntityType type, game_entity_id_type id) -> const std::shared_ptr<GameEntity>&;
        auto FindEntity(GameEntityType type, game_entity_id_type id) const -> const std::shared_ptr<GameEntity>&;

        auto GetDebugStatus() const -> std::string;

    private:
        void HandlePlayerAllState(const ZoneMessage& message);
        void HandlePlayerActivate(const ZoneMessage& message);

        void HandleRequestItemStructure(const ZoneMessage& message);

    private:
        const ServiceLocator& _serviceLocator;
        int32_t _stageId;

        std::unordered_map<int64_t, SharedPtrNotNull<GamePlayer>> _players;
        std::unordered_map<GameEntityType,
            boost::unordered::unordered_flat_map<game_entity_id_type, SharedPtrNotNull<GameEntity>>> _entities;

        static const std::shared_ptr<GameEntity> null_shared_entity;
    };
}
