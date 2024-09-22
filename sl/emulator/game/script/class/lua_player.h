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
    class LuaNPCTalkBox;
}

namespace sunlight
{
    class LuaPlayer
    {
    public:
        LuaPlayer(PlayerStateSystem& system, GamePlayer& player);

        void Talk(LuaNPC& npc, const LuaNPCTalkBox& talkBox);

        void DisposeTalk();

        auto GetSelection() const -> int32_t;

    public:
        auto GetCId() const -> int64_t;

    public:
        static void Bind(sol::state& luaState);

    private:
        PlayerStateSystem& _system;
        GamePlayer& _player;
    };
}
