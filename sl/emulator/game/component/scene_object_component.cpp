#include "scene_object_component.h"

namespace sunlight
{
    bool SceneObjectComponent::IsMoving() const
    {
        return _unk3 == 0x10;
    }

    auto SceneObjectComponent::GetId() const -> int32_t
    {
        return _id;
    }

    auto SceneObjectComponent::GetStageId() const -> int32_t
    {
        return _stageId;
    }

    auto SceneObjectComponent::GetPosition() const -> const Eigen::Vector3f&
    {
        return _position;
    }

    auto SceneObjectComponent::GetDestPosition() const -> const Eigen::Vector3f&
    {
        return _destPosition;
    }

    auto SceneObjectComponent::GetYaw() const -> float
    {
        return _yaw;
    }

    auto SceneObjectComponent::GetSpeed() const -> float
    {
        return _speed;
    }

    auto SceneObjectComponent::GetUnk1() const -> float
    {
        return _unk1;
    }

    auto SceneObjectComponent::GetUnk2() const -> uint16_t
    {
        return _unk2;
    }

    auto SceneObjectComponent::GetUnk3() const -> uint16_t
    {
        return _unk3;
    }

    void SceneObjectComponent::SetId(int32_t id)
    {
        _id = id;
    }

    void SceneObjectComponent::SetStageId(int32_t id)
    {
        _stageId = id;
    }

    void SceneObjectComponent::SetPosition(const Eigen::Vector3f& pos)
    {
        _position = pos;
    }

    void SceneObjectComponent::SetDestPosition(const Eigen::Vector3f& pos)
    {
        _destPosition = pos;
    }

    void SceneObjectComponent::SetYaw(float yaw)
    {
        _yaw = yaw;
    }

    void SceneObjectComponent::SetSpeed(float speed)
    {
        _speed = speed;
    }

    void SceneObjectComponent::SetUnk1(float unk1)
    {
        _unk1 = unk1;
    }

    void SceneObjectComponent::SetUnk2(uint16_t value)
    {
        _unk2 = value;
    }

    void SceneObjectComponent::SetUnk3(uint16_t value)
    {
        _unk3 = value;
    }
}
