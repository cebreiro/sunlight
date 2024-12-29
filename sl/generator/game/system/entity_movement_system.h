#pragma once
#include <boost/unordered/unordered_flat_map.hpp>

#include "sl/generator/game/entity/game_entity_id_type.h"
#include "sl/generator/game/system/game_system.h"

namespace sunlight
{
    struct ZoneRequest;

    class GameEntity;
    class SceneObjectSystem;
}

namespace sunlight
{
    class EntityMovementSystem final : public GameSystem
    {
    public:
        void InitializeSubSystem(Stage& stage) override;
        bool Subscribe(Stage& stage) override;

        bool ShouldUpdate() const override;
        void Update() override;

        auto GetName() const -> std::string_view override;
        auto GetClassId() const -> game_system_id_type override;

    public:
        bool Remove(game_entity_id_type id);

    public:
        void Teleport(GameEntity& entity, Eigen::Vector2f position);
        void MoveToPosition(GameEntity& entity, Eigen::Vector2f position, float speed);
        void MoveToTarget(GameEntity& entity, const GameEntity& target, float speed);

    private:
        void HandleMovement(const ZoneRequest& request);

        static auto CalculateYaw(const Eigen::Vector2f& from, const Eigen::Vector2f& to) -> float;

    private:
        void NotifyNewPosition(const GameEntity& entity, Eigen::Vector2f position);

    private:
        boost::unordered::unordered_flat_map<game_entity_id_type, PtrNotNull<GameEntity>> _movingEntities;
    };
}
