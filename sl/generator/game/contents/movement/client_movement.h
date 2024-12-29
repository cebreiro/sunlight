#pragma once

namespace sunlight
{
    struct ClientMovement
    {
        Eigen::Vector2f position = {};
        float yaw = 0.f;
        float speed = 0.f;
        float unk1 = 0.f;
        Eigen::Vector2f destPosition = {};
        uint16_t unk2 = 0;
        uint16_t movementTypeBitMask = 0;

        static auto CreateFrom(BufferReader& reader) -> ClientMovement;
    };
}
