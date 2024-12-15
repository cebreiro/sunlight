#include "game_debugger.h"

#include "sl/generator/game/time/game_time_service.h"

namespace sunlight
{
    thread_local GameDebugger* GameDebugger::_instance = nullptr;

    auto GameDebugger::GetName() const -> std::string_view
    {
        return "game_debugger";
    }

    bool GameDebugger::HasDebugTarget() const
    {
        return !_targets.empty();
    }

    bool GameDebugger::IsDebugTarget(GameDebugType type) const
    {
        return _targets.contains(type);
    }

    bool GameDebugger::UpdateLastReportTimePoint(GameDebugType type, std::chrono::milliseconds interval)
    {
        const game_time_point_type now = GameTimeService::Now();

        const auto iter = _lastReportTimePoints.find(type);
        if (iter == _lastReportTimePoints.end())
        {
            _lastReportTimePoints[type] = now;

            return true;
        }

        if (now >= iter->second + interval)
        {
            iter->second = now;

            return true;
        }

        return false;
    }

    void GameDebugger::Log(const std::string& str)
    {
        std::cout << fmt::format("{}\n", str);
    }

    void GameDebugger::SetState(GameDebugType type, bool value)
    {
        if (value)
        {
            _targets.insert(type);
        }
        else
        {
            _targets.erase(type);
        }
    }

    void GameDebugger::Clear()
    {
        _targets.clear();
    }

    bool GameDebugger::IsActive()
    {
        return _instance != nullptr;
    }

    bool GameDebugger::IsWatchTo(GameDebugType type)
    {
        assert(_instance);

        return _instance->IsDebugTarget(type);
    }

    bool GameDebugger::UpdateTimePoint(GameDebugType type, std::chrono::milliseconds interval)
    {
        assert(_instance);

        return _instance->UpdateLastReportTimePoint(type, interval);
    }

    void GameDebugger::Report(const std::string& str)
    {
        assert(_instance);

        _instance->Log(str);
    }

    void GameDebugger::SetInstance(GameDebugger* instance)
    {
        _instance = instance;
    }
}
