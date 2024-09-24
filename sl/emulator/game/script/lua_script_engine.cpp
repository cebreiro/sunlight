#include "lua_script_engine.h"

#include <boost/lexical_cast.hpp>
#include <sol/sol.hpp>

#include "sl/emulator/game/script/class/lua_player.h"
#include "sl/emulator/game/script/class/lua_npc.h"
#include "sl/emulator/game/script/class/lua_system.h"
#include "sl/emulator/game/script/lua_script_binder.h"

namespace sunlight
{
    LuaScriptEngine::LuaScriptEngine(const ServiceLocator& serviceLocator, const std::filesystem::path& scriptPath)
        : _serviceLocator(serviceLocator)
        , _scriptPath(scriptPath)
        , _npcScriptPath(_scriptPath / "npc")
    {
        try
        {
            LuaScriptBinder::Bind(_luaState);

            InitializeNPCScript(_npcScripts, _npcScriptPath);
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
        decltype(_npcScripts) newScript;
        if (!InitializeNPCScript(newScript, _npcScriptPath))
        {
            return false;
        }

        std::swap(_npcScripts, newScript);

        return true;
    }

    bool LuaScriptEngine::ExecuteNPCScript(int32_t scriptId, LuaSystem& system, LuaNPC& npc, LuaPlayer& player, int32_t sequence)
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

    bool LuaScriptEngine::InitializeNPCScript(std::unordered_map<int32_t, sol::protected_function>& outNpcScript, const std::filesystem::path& directory)
    {
        bool success = true;

        for (const auto& entry : std::filesystem::directory_iterator(directory))
        {
            const std::filesystem::path& path = entry.path();
            if (!path.has_extension() || ::_stricmp(path.extension().string().c_str(), ".lua") != 0)
            {
                continue;
            }

            const int32_t npcId = boost::lexical_cast<int32_t>(path.stem().string());

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

            outNpcScript[npcId] = function;
        }

        return success;
    }
}
