#pragma once
#include <sol/sol.hpp>

namespace sunlight
{
    class Quest;

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
        bool ExecuteNPCScript(int64_t scriptId, LuaSystem& system, LuaNPC& npc, LuaPlayer& player, int32_t sequence);
        bool ExecuteQuestScriptMonsterKill(LuaSystem& system, LuaPlayer& player, Quest& quest, int32_t monsterId);
        
    private:
        bool InitializeNgs(const std::filesystem::path& directory);
        bool InitializeCommandScript(std::unordered_map<std::string, sol::protected_function>& outScript, const std::filesystem::path& directory);
        bool InitializeNPCScript(std::unordered_map<int64_t, sol::protected_function>& outScript, const std::filesystem::path& directory);
        bool InitializeQuestScript(std::unordered_map<int32_t, sol::table>& outScript, const std::filesystem::path& directory);

    private:
        const ServiceLocator& _serviceLocator;
        std::filesystem::path _scriptPath;
        std::filesystem::path _ngsScriptPath;
        std::filesystem::path _commandScriptPath;
        std::filesystem::path _npcScriptPath;
        std::filesystem::path _questScriptPath;

        sol::state _luaState;
        std::unordered_map<std::string, sol::protected_function> _commandScripts;
        std::unordered_map<int64_t, sol::protected_function> _npcScripts;
        std::unordered_map<int32_t, sol::table> _questScripts;
    };
}
