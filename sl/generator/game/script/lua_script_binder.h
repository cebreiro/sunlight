#pragma once

namespace sol
{
    class state;
}

namespace sunlight
{
    class LuaScriptBinder
    {
    public:
        LuaScriptBinder() = delete;

        static void Bind(sol::state& luaState);

    private:
        static void BindPacket(sol::state& luaState);
    };
}
