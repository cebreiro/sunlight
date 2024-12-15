#pragma once

namespace sol
{
    class state;
}

namespace sunlight
{
    class GameScriptSystem;
}

namespace sunlight
{
    class LuaSystem
    {
    public:
        explicit LuaSystem(GameScriptSystem& system);

        void Debug(const std::string& str);
        void Info(const std::string& str);
        void Warn(const std::string& str);
        void Error(const std::string& str);

        auto GetSeconds() const -> int32_t;
        auto GetTimePoint() const -> int64_t;

    public:
        static void Bind(sol::state& luaState);

    private:
        GameScriptSystem& _system;
    };
}
