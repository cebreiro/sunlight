#include "lua_script_engine.h"

#include <boost/lexical_cast.hpp>
#include <sol/sol.hpp>

#include "sl/generator/game/script/class/lua_player.h"
#include "sl/generator/game/script/class/lua_npc.h"
#include "sl/generator/game/script/class/lua_system.h"
#include "sl/generator/game/script/lua_script_binder.h"

namespace sunlight
{
    LuaScriptEngine::LuaScriptEngine(const ServiceLocator& serviceLocator, const std::filesystem::path& scriptPath)
        : _serviceLocator(serviceLocator)
        , _scriptPath(scriptPath)
        , _ngsScriptPath(_scriptPath / "ngs")
        , _commandScriptPath(_scriptPath / "command")
        , _npcScriptPath(_scriptPath / "npc")
        , _questScriptPath(_scriptPath / "quest")
    {
        try
        {
            const std::initializer_list<sol::lib> libraries{
                sol::lib::base,
                sol::lib::math,
                sol::lib::string,
                sol::lib::table,
                sol::lib::package,
            };

            for (const sol::lib library : libraries)
            {
                _luaState.open_libraries(library);
            }

            _luaState["package"]["path"] = _luaState["package"]["path"].get<std::string>() + fmt::format(";{}/?.lua", _ngsScriptPath.string());

            LuaScriptBinder::Bind(_luaState);

            InitializeNgs(_ngsScriptPath);
            InitializeCommandScript(_commandScripts, _npcScriptPath);
            InitializeNPCScript(_npcScripts, _npcScriptPath);
            InitializeQuestScript(_questScripts, _questScriptPath);
        }
        catch (const std::exception& e)
        {
            SUNLIGHT_LOG_CRITICAL(_serviceLocator,
                fmt::format("[{}] fail to initialize. exception: {}",
                    GetName(), e.what()));
        }
        
    }

    LuaScriptEngine::~LuaScriptEngine()
    {
    }

    auto LuaScriptEngine::GetName() const -> std::string_view
    {
        return "lua_script_engine";
    }

    bool LuaScriptEngine::Reload()
    {
        decltype(_commandScripts) newCommandScripts;
        if (!InitializeCommandScript(newCommandScripts, _commandScriptPath))
        {
            return false;
        }

        decltype(_npcScripts) newScript;
        if (!InitializeNPCScript(newScript, _npcScriptPath))
        {
            return false;
        }

        decltype(_questScripts) newQuestScript;
        if (!InitializeQuestScript(newQuestScript, _questScriptPath))
        {
            return false;
        }

        std::swap(_commandScripts, newCommandScripts);
        std::swap(_npcScripts, newScript);
        std::swap(_questScripts, newQuestScript);

        return true;
    }

    bool LuaScriptEngine::ReloadCommandScript()
    {
        decltype(_commandScripts) newCommandScripts;
        if (!InitializeCommandScript(newCommandScripts, _commandScriptPath))
        {
            return false;
        }

        std::swap(_commandScripts, newCommandScripts);

        return true;
    }

    bool LuaScriptEngine::ExecuteCommandScript(const std::string& name, LuaPlayer& player)
    {
        const auto iter = _commandScripts.find(name);
        if (iter == _commandScripts.end())
        {
            return false;
        }

        sol::protected_function_result result = iter->second(&player);
        if (!result.valid())
        {
            const sol::error error = result;

            SUNLIGHT_LOG_ERROR(_serviceLocator,
                fmt::format("[{}] fail to execute script. player: {}, script: {}, error: {}",
                    GetName(), player.GetCId(), name, error.what()));

            return false;
        }

        return true;
    }

    bool LuaScriptEngine::ExecuteNPCScript(int64_t scriptId, LuaSystem& system, LuaNPC& npc, LuaPlayer& player, int32_t sequence)
    {
        const auto iter = _npcScripts.find(scriptId);
        if (iter == _npcScripts.end())
        {
            return false;
        }

        sol::protected_function_result result = iter->second(&system, &npc, &player, sequence);
        if (!result.valid())
        {
            const sol::error error = result;

            SUNLIGHT_LOG_ERROR(_serviceLocator,
                fmt::format("[{}] fail to execute script. player: {}, npc: {}, script: {}, error: {}",
                    GetName(), player.GetCId(), npc.GetId(), scriptId, error.what()));

            return false;
        }

        return true;
    }

    bool LuaScriptEngine::ExecuteQuestScriptMonsterKill(LuaSystem& system, LuaPlayer& player, Quest& quest, int32_t monsterId)
    {
        const auto iter = _questScripts.find(quest.GetId());
        if (iter == _questScripts.end())
        {
            return false;
        }

        if (const sol::function& function = iter->second["handleMonsterKill"];
            function.valid())
        {
            sol::protected_function_result result = function(system, player, quest, monsterId);
            if (!result.valid())
            {
                const sol::error error = result;

                SUNLIGHT_LOG_ERROR(_serviceLocator,
                    fmt::format("[{}] fail to execute script. player: {}, quest: {}, error: {}",
                        GetName(), player.GetCId(), quest.GetId(), error.what()));

                return false;
            }

            return true;
        }

        return false;
    }

    bool LuaScriptEngine::InitializeNgs(const std::filesystem::path& directory)
    {
        bool success = true;

        for (const auto& entry : std::filesystem::directory_iterator(directory))
        {
            const std::filesystem::path& path = entry.path();
            if (!path.has_extension() || ::_stricmp(path.extension().string().c_str(), ".lua") != 0)
            {
                continue;
            }

            sol::load_result script = _luaState.load_file(path.string());
            if (!script.valid())
            {
                success = false;
                const sol::error error = script;

                SUNLIGHT_LOG_CRITICAL(_serviceLocator,
                    fmt::format("[{}] fail to load file. path: {}, error: {}",
                        GetName(), path.string(), error.what()));
            }
        }

        return success;
    }

    bool LuaScriptEngine::InitializeCommandScript(std::unordered_map<std::string, sol::protected_function>& outScript, const std::filesystem::path& directory)
    {
        bool success = true;

        for (const auto& entry : std::filesystem::directory_iterator(directory))
        {
            const std::filesystem::path& path = entry.path();
            if (!path.has_extension() || ::_stricmp(path.extension().string().c_str(), ".lua") != 0)
            {
                continue;
            }

            const std::string& name = path.stem().string();

            sol::load_result script = _luaState.load_file(path.string());
            if (!script.valid())
            {
                success = false;
                const sol::error error = script;

                SUNLIGHT_LOG_CRITICAL(_serviceLocator,
                    fmt::format("[{}] fail to load file. path: {}, error: {}",
                        GetName(), path.string(), error.what()));

                continue;
            }

            sol::protected_function_result result = script();
            if (!result.valid())
            {
                success = false;
                const sol::error error = result;

                SUNLIGHT_LOG_CRITICAL(_serviceLocator,
                    fmt::format("[{}] fail to file. path: {}, error: {}",
                        GetName(), path.string(), error.what()));

                continue;
            }

            sol::protected_function function = result;
            if (!function.valid())
            {
                success = false;

                SUNLIGHT_LOG_CRITICAL(_serviceLocator,
                    fmt::format("[{}] npc talk script does't return function. path: {}",
                        GetName(), path.string()));

                continue;
            }

            outScript[name] = function;
        }

        return success;
    }

    bool LuaScriptEngine::InitializeNPCScript(std::unordered_map<int64_t, sol::protected_function>& outScript, const std::filesystem::path& directory)
    {
        bool success = true;

        for (const auto& entry : std::filesystem::recursive_directory_iterator(directory))
        {
            const std::filesystem::path& path = entry.path();
            if (!path.has_extension() || ::_stricmp(path.extension().string().c_str(), ".lua") != 0)
            {
                continue;
            }

            const int64_t npcId = boost::lexical_cast<int64_t>(path.stem().string());

            sol::load_result script = _luaState.load_file(path.string());
            if (!script.valid())
            {
                success = false;
                const sol::error error = script;

                SUNLIGHT_LOG_CRITICAL(_serviceLocator,
                    fmt::format("[{}] fail to load file. path: {}, error: {}",
                        GetName(), path.string(), error.what()));

                continue;
            }

            sol::protected_function_result result = script();
            if (!result.valid())
            {
                success = false;
                const sol::error error = result;

                SUNLIGHT_LOG_CRITICAL(_serviceLocator,
                    fmt::format("[{}] fail to file. path: {}, error: {}",
                        GetName(), path.string(), error.what()));

                continue;
            }

            sol::protected_function function = result;
            if (!function.valid())
            {
                success = false;

                SUNLIGHT_LOG_CRITICAL(_serviceLocator,
                    fmt::format("[{}] npc talk script does't return function. path: {}",
                        GetName(), path.string()));

                continue;
            }

            outScript[npcId] = function;
        }

        return success;
    }

    bool LuaScriptEngine::InitializeQuestScript(std::unordered_map<int32_t, sol::table>& outScript, const std::filesystem::path& directory)
    {
        bool success = true;

        for (const auto& entry : std::filesystem::recursive_directory_iterator(directory))
        {
            const std::filesystem::path& path = entry.path();
            if (!path.has_extension() || ::_stricmp(path.extension().string().c_str(), ".lua") != 0)
            {
                continue;
            }

            const int32_t questId = boost::lexical_cast<int32_t>(path.stem().string());

            sol::load_result script = _luaState.load_file(path.string());
            if (!script.valid())
            {
                success = false;
                const sol::error error = script;

                SUNLIGHT_LOG_CRITICAL(_serviceLocator,
                    fmt::format("[{}] fail to load file. path: {}, error: {}",
                        GetName(), path.string(), error.what()));

                continue;
            }

            sol::protected_function_result result = script();
            if (!result.valid())
            {
                success = false;
                const sol::error error = result;

                SUNLIGHT_LOG_CRITICAL(_serviceLocator,
                    fmt::format("[{}] fail to file. path: {}, error: {}",
                        GetName(), path.string(), error.what()));

                continue;
            }

            outScript[questId] = result;
        }

        return success;
    }
}
