#pragma once
#include <sol/sol.hpp>

namespace sunlight
{
    class LuaPlayer;
    class LuaNPC;
}
namespace sunlight
{
    class LuaScriptEngine final : public IService
    {
    public:
        LuaScriptEngine(const LuaScriptEngine& other) = delete;
        LuaScriptEngine(LuaScriptEngine&& other) noexcept = delete;
        LuaScriptEngine& operator=(const LuaScriptEngine& other) = delete;
        LuaScriptEngine& operator=(LuaScriptEngine&& other) noexcept = delete;

    public:
        LuaScriptEngine(const ServiceLocator& serviceLocator, const std::filesystem::path& scriptPath);
        ~LuaScriptEngine();

        auto GetName() const -> std::string_view override;

    public:
        bool ExecuteNPCScript(int32_t scriptId, LuaNPC& npc, LuaPlayer& player, int32_t sequence);
        
    private:
        void InitializeNPCScript(const std::filesystem::path& directory);

    private:
        const ServiceLocator& _serviceLocator;
        const std::filesystem::path& _scriptPath;

        sol::state _luaState;
        std::unordered_map<int32_t, sol::protected_function> _npcScripts;
    };
}
