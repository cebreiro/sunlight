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

        // move bitmask client 0x058EC00, 0x590B90
        // 0b00010000 / move // move to dest position?
        // 0b00000010 / unk1 // move backward (yaw) 
        // 0b00000001 / unk2 // move forward  (yaw)
        // 0b00000000 / stop
        uint16_t movementTypeBitMask = 0;

        static auto CreateFrom(BufferReader& reader) -> ClientMovement;
    };
}
