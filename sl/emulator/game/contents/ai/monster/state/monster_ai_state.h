#pragma once

namespace sunlight
{
    ENUM_CLASS(MonsterAIStateType, int32_t,
        (Spawn)
        (Wander)
        (Combat)
        (Return)
    )
}
