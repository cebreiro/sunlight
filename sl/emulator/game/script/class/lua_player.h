#pragma once

namespace sol
{
    class state;
}

namespace sunlight
{
    class PlayerStateSystem;
    class GamePlayer;

    class LuaNPC;
    class NPCTalkBox;
}

namespace sunlight
{
    class LuaPlayer
    {
    public:
        LuaPlayer(PlayerStateSystem& system, GamePlayer& player);

        void Talk(LuaNPC& npc, const NPCTalkBox& talkBox);

        void DisposeTalk();

        auto GetSelection() const -> int32_t;

    public:
        auto GetCId() const -> int64_t;

    private:
        PlayerStateSystem& _system;
        GamePlayer& _player;
    };
}
