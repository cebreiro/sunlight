#pragma once
#include "sl/emulator/game/component/game_component.h"
#include "sl/emulator/game/contents/movement/forward_movement.h"

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
        auto GetUnk3() const -> uint16_t;
        auto GetMovement() const -> const ForwardMovement&;

        void SetId(int32_t id);
        void SetPosition(const Eigen::Vector2f& pos);
        void SetDestPosition(const Eigen::Vector2f& pos);
        void SetYaw(float yaw);
        void SetSpeed(float speed);
        void SetUnk1(float unk1);
        void SetUnk2(uint16_t value);
        void SetUnk3(uint16_t value);

        void Set(const ForwardMovement& movement);
        void SetMoving(bool value);

    private:
        int32_t _id = 0;

        ForwardMovement _forwardMovement;
    };
}
