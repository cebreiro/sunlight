#include "scene_object_component.h"

namespace sunlight
{
    bool SceneObjectComponent::IsMoving() const
    {
        return _forwardMovement.unk3 == 0x10;
    }

    auto SceneObjectComponent::GetId() const -> int32_t
    {
        return _id;
    }

    auto SceneObjectComponent::GetPosition() const -> const Eigen::Vector2f&
    {
        return _forwardMovement.position;
    }

    auto SceneObjectComponent::GetDestPosition() const -> const Eigen::Vector2f&
    {
        return _forwardMovement.destPosition;
    }

    auto SceneObjectComponent::GetYaw() const -> float
    {
        return _forwardMovement.yaw;
    }

    auto SceneObjectComponent::GetSpeed() const -> float
    {
        return _forwardMovement.speed;
    }

    auto SceneObjectComponent::GetUnk1() const -> float
    {
        return _forwardMovement.unk1;
    }

    auto SceneObjectComponent::GetUnk2() const -> uint16_t
    {
        return _forwardMovement.unk2;
    }

    auto SceneObjectComponent::GetUnk3() const -> uint16_t
    {
        return _forwardMovement.unk3;
    }

    void SceneObjectComponent::SetId(int32_t id)
    {
        _id = id;
    }

    void SceneObjectComponent::SetPosition(const Eigen::Vector2f& pos)
    {
        _forwardMovement.position = pos;
    }

    void SceneObjectComponent::SetDestPosition(const Eigen::Vector2f& pos)
    {
        _forwardMovement.destPosition = pos;
    }

    void SceneObjectComponent::SetYaw(float yaw)
    {
        _forwardMovement.yaw = yaw;
    }

    void SceneObjectComponent::SetSpeed(float speed)
    {
        _forwardMovement.speed = speed;
    }

    void SceneObjectComponent::SetUnk1(float unk1)
    {
        _forwardMovement.unk1 = unk1;
    }

    void SceneObjectComponent::SetUnk2(uint16_t value)
    {
        _forwardMovement.unk2 = value;
    }

    void SceneObjectComponent::SetUnk3(uint16_t value)
    {
        _forwardMovement.unk3 = value;
    }

    void SceneObjectComponent::Set(const ForwardMovement& movement)
    {
        _forwardMovement = movement;
    }

    void SceneObjectComponent::SetMoving(bool value)
    {
        _forwardMovement.unk3 = value ? 0x10 : 0;
    }
}
