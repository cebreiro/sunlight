#pragma once

namespace sol
{
    class state;
}

namespace sunlight
{
    class GameMonster;
}


namespace sunlight
{
    class LuaMonster
    {
    public:
        LuaMonster(const LuaMonster& other) = delete;
        LuaMonster& operator=(const LuaMonster& other) = delete;
        LuaMonster(LuaMonster&& other) noexcept = default;
        LuaMonster& operator=(LuaMonster&& other) noexcept = default;

    public:
        explicit LuaMonster(GameMonster& monster);

        auto GetId() const -> int32_t;
        auto GetTypeValue() const -> int32_t;

    public:
        static void Bind(sol::state& luaState);

    private:
        PtrNotNull<GameMonster> _monster = nullptr;
    };
}
