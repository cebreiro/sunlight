#pragma once
#include "sl/generator/game/debug/game_debug_type.h"
#include "sl/generator/game/time/game_time.h"

namespace sunlight
{
    class GameDebugger final : public IService
    {
    public:
        auto GetName() const -> std::string_view override;

    public:
        bool HasDebugTarget() const;
        bool IsDebugTarget(GameDebugType type) const;

        bool UpdateLastReportTimePoint(GameDebugType type, std::chrono::milliseconds interval);

        void Log(const std::string& str);

        void SetState(GameDebugType type, bool value);
        void Clear();

    public:
        static bool IsActive();
        static bool IsWatchTo(GameDebugType type);
        static bool UpdateTimePoint(GameDebugType type, std::chrono::milliseconds interval);

        static void Report(const std::string& str);

    public:
        static void SetInstance(GameDebugger* instance);

    private:
        std::unordered_set<GameDebugType> _targets;
        std::unordered_map<GameDebugType, game_time_point_type> _lastReportTimePoints;

        static thread_local GameDebugger* _instance;
    };
}

#define SUNLIGHT_GAME_DEBUG_REPORT(type, str)\
if (GameDebugger::IsActive() && GameDebugger::IsWatchTo(type)) \
    GameDebugger::Report(str)

#define SUNLIGHT_GAME_DEBUG_REPORT_INTERVAL(type, interval, str)\
if (GameDebugger::IsActive() && GameDebugger::IsWatchTo(type) && GameDebugger::UpdateTimePoint(type, interval) ) \
    GameDebugger::Report(str)
