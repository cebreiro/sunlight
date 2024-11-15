#pragma once
#include <sol/sol.hpp>

namespace sunlight
{
    class LuaSystem;
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
        bool Reload();
        bool ReloadCommandScript();

        bool ExecuteCommandScript(const std::string& name, LuaPlayer& player);
        bool ExecuteNPCScript(int32_t scriptId, LuaSystem& system, LuaNPC& npc, LuaPlayer& player, int32_t sequence);
        
    private:
        bool InitializeNgs(const std::filesystem::path& directory);
        bool InitializeCommandScript(std::unordered_map<std::string, sol::protected_function>& outScript, const std::filesystem::path& directory);
        bool InitializeNPCScript(std::unordered_map<int32_t, sol::protected_function>& outScript, const std::filesystem::path& directory);

    private:
        const ServiceLocator& _serviceLocator;
        std::filesystem::path _scriptPath;
        std::filesystem::path _ngsScriptPath;
        std::filesystem::path _commandScriptPath;
        std::filesystem::path _npcScriptPath;

        sol::state _luaState;
        std::unordered_map<std::string, sol::protected_function> _commandScripts;
        std::unordered_map<int32_t, sol::protected_function> _npcScripts;
    };
}
