#pragma once

namespace sunlight
{
    ENUM_CLASS(ItemMixOpenByItemFailReason, int32_t,
        (NoItemMixSkill, 0)
        (InvalidState, 1)
        (NotTerrain, 2)
    )
}
