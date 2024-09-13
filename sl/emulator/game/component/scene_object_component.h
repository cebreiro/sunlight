#pragma once
#include "sl/emulator/game/component/game_component.h"

namespace sunlight
{
    class SceneObjectComponent final : public GameComponent
    {
    public:
        bool IsMoving() const;

        auto GetId() const -> int32_t;
        auto GetStageId() const -> int32_t;
        auto GetPosition() const -> const Eigen::Vector3f&;
        auto GetDestPosition() const -> const Eigen::Vector3f&;
        auto GetYaw() const -> float;
        auto GetSpeed() const -> float;
        auto GetUnk1() const -> float;
        auto GetUnk2() const -> uint16_t;
        auto GetUnk3() const -> uint16_t;

        void SetId(int32_t id);
        void SetStageId(int32_t id);
        void SetPosition(const Eigen::Vector3f& pos);
        void SetDestPosition(const Eigen::Vector3f& pos);
        void SetYaw(float yaw);
        void SetSpeed(float speed);
        void SetUnk1(float unk1);
        void SetUnk2(uint16_t value);
        void SetUnk3(uint16_t value);

    private:
        int32_t _id = 0;
        int32_t _stageId = 0;

        Eigen::Vector3f _position = {};
        float _yaw = 0.f;
        float _speed = 0.f;
        float _unk1 = 0.f;
        Eigen::Vector3f _destPosition = {};
        uint16_t _unk2 = 0;
        uint16_t _unk3 = 0; // maybe move type. 0-stop, 0x10: move
    };
}
