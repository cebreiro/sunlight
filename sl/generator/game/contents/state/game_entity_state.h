#pragma once
#include "sl/generator/game/contents/state/game_entity_state_type.h"
#include "sl/generator/game/entity/game_entity_id_type.h"
#include "sl/generator/game/entity/game_entity_type.h"

namespace sunlight
{
    struct GameEntityState
    {
        enum class MoveType : int8_t
        {
            Stop = -1,
            Run = 0,
            Walk = 1,
        };

        uint8_t bitmask1 = uint8_t{ 0xFF };
        uint8_t bitmask2 = uint8_t{ 0x0F };

        GameEntityStateType type = GameEntityStateType::None;
        MoveType moveType = MoveType::Stop;                  
        uint8_t unk4 = 0;                                    
        uint8_t unk5 = uint8_t{ 0xFF };                      
        Eigen::Vector3f destPosition = {};
        game_entity_id_type targetId = {};
        GameEntityType targetType = {};
        int32_t attackId = 0;
        int32_t motionId = 0;
        int32_t fxId = 0;
        int32_t param1 = 0;
        int32_t param2 = 0;
        int32_t skillId = 0;

        static auto CreateFrom(BufferReader& reader) -> GameEntityState;
    };
}
