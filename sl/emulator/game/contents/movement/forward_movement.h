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

        // move bitmask
        // 0b00010000 / move / 590B90(object_move) -> 58EAD
        // 0b00000001 / unk  / 590B90(object_move) -> 58EA90
        // 0b00000000 / stop
        uint16_t unk3 = 0;

        static auto CreateFrom(BufferReader& reader) -> ForwardMovement;
    };
}
