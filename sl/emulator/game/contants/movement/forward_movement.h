#pragma once

namespace sunlight
{
    struct ForwardMovement
    {
        Eigen::Vector2f position = {};
        float yaw = 0.f;
        float speed = 0.f;
        float unk1 = 0.f;
        Eigen::Vector2f destPosition = {};
        uint16_t unk2 = 0;
        uint16_t unk3 = 0; // maybe move type. 0-stop, 0x10: move

        static auto CreateFrom(BufferReader& reader) -> ForwardMovement;
    };
}
