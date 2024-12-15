#include "scene_object_component.h"

namespace sunlight
{
    bool SceneObjectComponent::IsMoving() const
    {
        return _clientMovement.movementTypeBitMask == 0x10;
    }

    auto SceneObjectComponent::GetId() const -> int32_t
    {
        return _id;
    }

    auto SceneObjectComponent::GetPosition() const -> const Eigen::Vector2f&
    {
        return _clientMovement.position;
    }

    auto SceneObjectComponent::GetDestPosition() const -> const Eigen::Vector2f&
    {
        return _clientMovement.destPosition;
    }

    auto SceneObjectComponent::GetYaw() const -> float
    {
        return _clientMovement.yaw;
    }

    auto SceneObjectComponent::GetSpeed() const -> float
    {
        return _clientMovement.speed;
    }

    auto SceneObjectComponent::GetUnk1() const -> float
    {
        return _clientMovement.unk1;
    }

    auto SceneObjectComponent::GetUnk2() const -> uint16_t
    {
        return _clientMovement.unk2;
    }

    auto SceneObjectComponent::GetMovementTypeBitMask() const -> uint16_t
    {
        return _clientMovement.movementTypeBitMask;
    }

    auto SceneObjectComponent::GetMovement() const -> const ClientMovement&
    {
        return _clientMovement;
    }

    auto SceneObjectComponent::GetBodySize() const -> int32_t
    {
        return _bodySize;
    }

    void SceneObjectComponent::SetId(int32_t id)
    {
        _id = id;
    }

    void SceneObjectComponent::SetPosition(const Eigen::Vector2f& pos)
    {
        _clientMovement.position = pos;
    }

    void SceneObjectComponent::SetDestPosition(const Eigen::Vector2f& pos)
    {
        _clientMovement.destPosition = pos;
    }

    void SceneObjectComponent::SetYaw(float yaw)
    {
        _clientMovement.yaw = yaw;
    }

    void SceneObjectComponent::SetSpeed(float speed)
    {
        _clientMovement.speed = speed;
    }

    void SceneObjectComponent::SetUnk1(float unk1)
    {
        _clientMovement.unk1 = unk1;
    }

    void SceneObjectComponent::SetUnk2(uint16_t value)
    {
        _clientMovement.unk2 = value;
    }

    void SceneObjectComponent::SetMovementTypeBitMask(uint16_t value)
    {
        _clientMovement.movementTypeBitMask = value;
    }

    void SceneObjectComponent::Set(const ClientMovement& movement)
    {
        _clientMovement = movement;
    }

    void SceneObjectComponent::SetMoving(bool value)
    {
        _clientMovement.movementTypeBitMask = value ? 0x10 : 0;
    }

    void SceneObjectComponent::SetBodySize(int32_t value)
    {
        _bodySize = value;
    }
}
