#pragma once

namespace sunlight
{
    ENUM_CLASS(GameEntityType, int32_t,
        (None, 0)
        (Player, 1100)
        (NPC, 1200)
        (Enemy, 1300)
        (EnemyChild, 1310)
        (FX, 1400)
        (House, 1500)
        (Prop, 1600)
        (PropChild, 1610)
        (Item, 1700)
        (ItemChild, 1710)
        (EventObj, 1800)
        (Stage, 1900)
        (StageTerrain, 1910)
        (StageRoom, 1920)
        (Widget, 200)
        (Unk1940, 0x794)
        (Summon, 2100)
        (Pet, 2300)
        (MiniPK, 2400)
        (Unk3100, 0xC1C)
        (Unk3200, 0xC80)
    )
}
