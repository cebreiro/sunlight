#pragma once

namespace sol
{
    class state;
}

namespace sunlight
{
    class GameNPC;

    class LuaPlayer;
}

namespace sunlight
{
    class LuaNPC
    {
        friend class LuaPlayer;

    public:
        LuaNPC(const LuaNPC& other) = delete;
        LuaNPC(LuaNPC&& other) noexcept = delete;
        LuaNPC& operator=(const LuaNPC& other) = delete;
        LuaNPC& operator=(LuaNPC&& other) noexcept = delete;

    public:
        explicit LuaNPC(GameNPC& npc);

    public:
        auto GetId() const -> int32_t;

    public:
        static void Bind(sol::state& luaState);

    private:
        GameNPC& _npc;
    };
}
