#pragma once
#include "sl/generator/game/entity/game_entity_id_type.h"
#include "sl/generator/game/system/game_system.h"

namespace sunlight
{
    struct MapStage;

    class GameEntity;
    class GamePlayer;
    class GameItem;
    class GameMonster;

    class GameSpatialSector;
    class GameSpatialCell;
}

namespace sunlight
{
    class EntityViewRangeSystem final : public GameSystem
    {
    public:
        EntityViewRangeSystem(const ServiceLocator& serviceLocator, const MapStage& stageData);
        ~EntityViewRangeSystem();

        void InitializeSubSystem(Stage& stage) override;
        bool Subscribe(Stage& stage) override;
        auto GetName() const -> std::string_view override;
        auto GetClassId() const -> game_system_id_type override;

    public:
        void Add(GameEntity& entity);
        void Remove(GameEntity& entity);

        bool IsAdjacent(const GameEntity& entity, const GameEntity& other) const;

        void UpdateViewRange(GameEntity& entity, const Eigen::Vector2f& newPosition);

        void VisitEntity(const Eigen::Vector2f& position, const std::function<void(GameEntity&)>& function);
        void VisitEntity(const GameEntity& centerEntity, const std::function<void(GameEntity&)>& function);
        void VisitPlayer(const Eigen::Vector2f& position, const std::function<void(GamePlayer&)>& function);
        void VisitPlayer(const GameEntity& centerEntity, const std::function<void(GamePlayer&)>& function);
        void VisitMonster(const GameEntity& centerEntity, const std::function<void(GameMonster&)>& function);

        void Broadcast(const Eigen::Vector2f& position, const Buffer& buffer);
        void Broadcast(const GameEntity& centerEntity, const Buffer& buffer, bool includeCenter);

        auto GetSector(const GameEntity& entity) -> GameSpatialSector&;
        auto GetSector(const GameEntity& entity) const -> const GameSpatialSector&;
        auto GetSector(int32_t x, int32_t y) -> GameSpatialSector&;
        auto GetSector(int32_t x, int32_t y) const -> const GameSpatialSector&;
        auto GetSector(const Eigen::Vector2f& position) -> GameSpatialSector&;
        auto GetSector(const Eigen::Vector2f& position) const -> const GameSpatialSector&;

    private:
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

        // camera range is approximately 530.
        // create an entity outside of camera range by adding 128
        static constexpr int32_t cell_size = 512 + 128;

        std::vector<UniquePtrNotNull<GameSpatialSector>> _sectors;
        std::vector<UniquePtrNotNull<GameSpatialCell>> _cells;

        std::unordered_map<game_entity_id_type, PtrNotNull<GameSpatialSector>> _entitySectorIndex;
    };
}
