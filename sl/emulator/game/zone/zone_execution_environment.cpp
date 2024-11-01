#include "zone_execution_environment.h"

#include "sl/emulator/game/debug/game_debugger.h"
#include "sl/emulator/game/time/game_time.h"
#include "sl/emulator/game/time/game_time_service.h"

namespace sunlight
{
    ZoneExecutionEnvironment::ZoneExecutionEnvironment(const ServiceLocator& serviceLocator)
        : _serviceLocator(serviceLocator)
    {
        if (GameDebugger* debugger = _serviceLocator.Find<GameDebugger>(); debugger && debugger->HasDebugTarget())
        {
            GameDebugger::SetInstance(debugger);
        }

        GameTimeService::SetNow(game_clock_type::now());
    }

    ZoneExecutionEnvironment::~ZoneExecutionEnvironment()
    {
        GameDebugger::SetInstance(nullptr);
    }
}
