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
    public:
        LuaNPC(const LuaNPC& other) = delete;
        LuaNPC(LuaNPC&& other) noexcept = delete;
        LuaNPC& operator=(const LuaNPC& other) = delete;
        LuaNPC& operator=(LuaNPC&& other) noexcept = delete;

    public:
        explicit LuaNPC(GameNPC& npc);

    public:
        static void Bind(sol::state& luaState);

    public:
        auto GetId() const -> uint32_t;
        auto GetImpl() const -> GameNPC&;

    private:
        GameNPC& _npc;
    };
}
