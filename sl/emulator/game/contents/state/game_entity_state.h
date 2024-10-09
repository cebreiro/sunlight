#pragma once
#include "sl/emulator/game/contents/state/game_entity_state_type.h"
#include "sl/emulator/game/entity/game_entity_id_type.h"
#include "sl/emulator/game/entity/game_entity_type.h"

namespace sunlight
{
    // client
    // 0x491FB3, 0x48FE2A, 
    struct GameEntityState
    {
        enum class MoveType : int8_t
        {
            Stop = -1,
            Run = 0,
            Walk = 1,
        };

        // bit-masks to indicate either index field is used or not
        uint8_t bitmask1 = uint8_t{ 0xFF }; // 0 ~ 7
        uint8_t bitmask2 = uint8_t{ 0x0F }; // 8 ~ 11

        // client 491F67h
        // v21(type, moving, unk4 unk5) default initialization to (00 FF 00 FF)
        /*index: 0*/GameEntityStateType type = GameEntityStateType::None;
        /*index: 1*/MoveType moveType = MoveType::Stop;
        /*index: 2*/uint8_t unk4 = 0;
        /*index: 3*/uint8_t unk5 = uint8_t{ 0xFF };
        /*index: 4*/Eigen::Vector3f destPosition = {};
        /*index: 5*/game_entity_id_type targetId = {};
        /*index: 5*/GameEntityType targetType = {};
        /*index: 6*/int32_t attackId = 0;
        /*index: 7*/int32_t motionId = 0;
        /*index: 8*/int32_t fxId = 0;
        /*index: 9*/int32_t param1 = 0; // skill -> charge time
        /*index:10*/int32_t param2 = 0; // skill -> charge count
        /*index:11*/int32_t skillId = 0;

        static auto CreateFrom(BufferReader& reader) -> GameEntityState;
    };
}
