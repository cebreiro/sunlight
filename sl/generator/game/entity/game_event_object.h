#pragma once
#include "sl/generator/game/entity/game_entity.h"

namespace sunlight
{
    class GameEventObject final : public GameEntity
    {
    public:
        static constexpr GameEntityType TYPE = GameEntityType::EventObj;

        GameEventObject(game_entity_id_type id, Eigen::Vector2f center, float yaw, const Eigen::AlignedBox2f& area);

        auto GetCenterPosition() const -> Eigen::Vector2f;
        auto GetYaw() const -> float;
        auto GetArea() const -> const Eigen::AlignedBox2f&;

    private:
        Eigen::Vector2f _center = {};
        float _yaw = 0.f;
        Eigen::AlignedBox2f _area = {};
    };
}
