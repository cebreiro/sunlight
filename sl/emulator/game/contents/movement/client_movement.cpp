#include "client_movement.h"

namespace sunlight
{
    auto ClientMovement::CreateFrom(BufferReader& reader) -> ClientMovement
    {
        ClientMovement result;
        result.position.x() = reader.Read<float>();
        result.position.y() = reader.Read<float>();
        result.yaw = reader.Read<float>();
        result.speed = reader.Read<float>();
        result.unk1 = reader.Read<float>();
        result.destPosition.x() = reader.Read<float>();;
        result.destPosition.y() = reader.Read<float>();;
        result.unk2 = reader.Read<uint16_t>();
        result.movementTypeBitMask = reader.Read<uint16_t>();

        return result;
    }
}
