#include "zone_execution_environment.h"

#include "sl/generator/game/debug/game_debugger.h"
#include "sl/generator/game/time/game_time.h"
#include "sl/generator/game/time/game_time_service.h"

namespace sunlight
{
    thread_local std::vector<std::source_location> ZoneExecutionEnvironment::_sourceLocations;

    ZoneExecutionEnvironment::ZoneExecutionEnvironment(const ServiceLocator& serviceLocator, const std::source_location& sourceLocation)
        : _serviceLocator(serviceLocator)
    {
        _sourceLocations.emplace_back(sourceLocation);

        if (GameDebugger* debugger = _serviceLocator.Find<GameDebugger>(); debugger && debugger->HasDebugTarget())
        {
            GameDebugger::SetInstance(debugger);
        }

        GameTimeService::SetNow(game_clock_type::now());
    }

    ZoneExecutionEnvironment::~ZoneExecutionEnvironment()
    {
        GameDebugger::SetInstance(nullptr);

        _sourceLocations.pop_back();
    }
}
