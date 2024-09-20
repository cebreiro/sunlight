#pragma once

namespace sunlight
{
    // client 0x48E77C
    ENUM_CLASS(GameEntityStateType, int8_t,
        (None, 0)
        (Moving, 1)
        (NormalAttack, 2)
        (DamagedMotion, 4)
        (Conversation, 5)
        (PlaySkill, 7)
        (Dying, 8)
        (Dead, 9)
        (PickGroundItem, 10)
        (Disarming, 11)
        (Arming, 12)
        (Entering, 13)
        (Leaving, 14)
        (DamageCancel, 15)
        (Idle, 17)
        (Greet, 18)
        (InteractWithPlayer, 21)
        (Resurrection, 24)
        (ChangingZone, 25)
        (Hang, 28)
        (ChangingRoom, 29)
        (Sitting, 31)
        (StandingUp, 32)
        (UseItem, 33)
        (Emotion, 34)
        (UnkSkill, 36) // angel wings
    )
}
