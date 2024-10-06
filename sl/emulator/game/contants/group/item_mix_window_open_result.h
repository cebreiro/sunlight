#pragma once

namespace sunlight
{
    // client 0x458118
    ENUM_CLASS(ItemMixOpenByItemFailReason, int32_t,
        (NoItemMixSkill, 0)
        (InvalidState, 1)
        (NotTerrain, 2)
    )
}
