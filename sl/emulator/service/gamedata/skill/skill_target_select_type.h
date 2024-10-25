#pragma once

namespace sunlight
{
    ENUM_CLASS(SkillTargetSelectType, int32_t,
        (MonsterSelf, 0)
        (Monster, 1)
        (PlayerAlly, 2)
        (MaybeGroundPoint, 3)
        (Party, 5)
        (PlayerSelf, 6)
    );
}
