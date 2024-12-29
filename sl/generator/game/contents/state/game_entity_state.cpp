#include "game_entity_state.h"

namespace sunlight
{
    auto GameEntityState::CreateFrom(BufferReader& reader) -> GameEntityState
    {
        GameEntityState result;
        result.bitmask1 = reader.Read<uint8_t>();
        result.bitmask2 = reader.Read<uint8_t>();
        result.type = static_cast<GameEntityStateType>(reader.Read<int8_t>());
        result.moveType = static_cast<MoveType>(reader.Read<int8_t>());
        result.unk4 = reader.Read<uint8_t>();
        result.unk5 = reader.Read<uint8_t>();

        result.destPosition.x() = reader.Read<float>();
        result.destPosition.y() = reader.Read<float>();
        result.destPosition.z() = reader.Read<float>();

        result.targetId = game_entity_id_type(reader.Read<int32_t>());
        result.targetType = static_cast<GameEntityType>(reader.Read<int32_t>());
        result.attackId = reader.Read<int32_t>();
        result.motionId = reader.Read<int32_t>();
        result.fxId = reader.Read<int32_t>();
        result.param1 = reader.Read<int32_t>();
        result.param2 = reader.Read<int32_t>();
        result.skillId = reader.Read<int32_t>();

        return result;
    }
}
