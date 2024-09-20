#pragma once
#include "sl/emulator/game/debug/game_debug_type.h"

namespace sunlight
{
    class GameDebugger final : public IService
    {
    public:
        auto GetName() const -> std::string_view override;

    public:
        bool HasDebugTarget() const;
        bool IsDebugTarget(GameDebugType type) const;

        void Log(const std::string& str);

        void SetState(GameDebugType type, bool value);
        void Clear();

    public:
        static bool IsActive();
        static bool IsWatchTo(GameDebugType type);

        static void Report(const std::string& str);

    public:
        static void SetInstance(GameDebugger* instance);

    private:
        std::unordered_set<GameDebugType> _targets;

        static thread_local GameDebugger* _instance;
    };
}

#define SUNLIGHT_GAME_DEBUG_REPORT(type, str)\
if (GameDebugger::IsActive() && GameDebugger::IsWatchTo(type)) \
    GameDebugger::Report(str)
