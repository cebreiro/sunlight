#pragma once

namespace sunlight
{
    class PlayerSkill;

    class GamePlayer;
}

namespace sunlight
{
    struct EventBubblingPlayerSkillAdd
    {
        PtrNotNull<GamePlayer> player = nullptr;
        PtrNotNull<PlayerSkill> skill = nullptr;
    };

    struct EventBubblingPlayerSkillLevelChange
    {
        PtrNotNull<GamePlayer> player = nullptr;
        PtrNotNull<const PlayerSkill> skill = nullptr;

        int32_t oldLevel = 0;
        int32_t newLevel = 0;
    };

    struct EventBubblingPlayerQuestMonsterKill
    {
        PtrNotNull<GamePlayer> player = nullptr;

        int32_t questId = 0;
        int32_t monsterId = 0;
    };
}
