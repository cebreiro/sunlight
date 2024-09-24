#pragma once

namespace sol
{
    class state;
}

namespace sunlight
{
    class PlayerStateSystem;
}

namespace sunlight
{
    class LuaSystem
    {
    public:
        explicit LuaSystem(PlayerStateSystem& system);

        void Print(const std::string& str);

        auto GetTimePoint() const -> int64_t;

    public:
        static void Bind(sol::state& luaState);

    private:
        PlayerStateSystem& _system;
    };
}
