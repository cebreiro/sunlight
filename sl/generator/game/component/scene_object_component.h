#pragma once
#include "sl/generator/game/component/game_component.h"
#include "sl/generator/game/contents/movement/client_movement.h"

namespace sunlight
{
    class SceneObjectComponent final : public GameComponent
    {
    public:
        bool IsMoving() const;

        auto GetId() const -> int32_t;
        auto GetPosition() const -> const Eigen::Vector2f&;
        auto GetDestPosition() const -> const Eigen::Vector2f&;
        auto GetYaw() const -> float;
        auto GetSpeed() const -> float;
        auto GetUnk1() const -> float;
        auto GetUnk2() const -> uint16_t;
        auto GetMovementTypeBitMask() const -> uint16_t;
        auto GetMovement() const -> const ClientMovement&;
        auto GetBodySize() const -> int32_t;

        void SetId(int32_t id);
        void SetPosition(const Eigen::Vector2f& pos);
        void SetDestPosition(const Eigen::Vector2f& pos);
        void SetYaw(float yaw);
        void SetSpeed(float speed);
        void SetUnk1(float unk1);
        void SetUnk2(uint16_t value);
        void SetMovementTypeBitMask(uint16_t value);

        void Set(const ClientMovement& movement);
        void SetMoving(bool value);
        void SetBodySize(int32_t value);

    private:
        int32_t _id = 0;
        int32_t _bodySize = 0;

        ClientMovement _clientMovement;
    };
}
