#pragma once

namespace sunlight
{
    class GameMonster;
}

namespace sunlight
{
    struct EventBubblingMonsterSpawn
    {
        PtrNotNull<GameMonster> monster = nullptr;
    };

    struct EventBubblingMonsterDespawn
    {
        PtrNotNull<GameMonster> monster = nullptr;
    };
}
