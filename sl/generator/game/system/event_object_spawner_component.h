#pragma once
#include "sl/generator/game/component/game_component.h"

namespace sunlight
{
    class MonsterData;
}

namespace sunlight
{
    struct SpawnerContext
    {
        PtrNotNull<const MonsterData> data = nullptr;
        int32_t maxCount = 0;
        int32_t firstDelay = 0;
        int32_t delay = 0;
        int32_t spawnCount = 0;

        bool timerRunning = false;
    };

    class EventObjectSpawnerComponent final : public GameComponent
    {
    public:
        EventObjectSpawnerComponent() = default;

        void AddContext(const MonsterData& data, int32_t maxCount, int32_t firstDelay, int32_t delay);

        auto FindContext(int32_t mobId) -> SpawnerContext*;
        auto FindContext(int32_t mobId) const -> const SpawnerContext*;

        inline auto GetSpawnDataRange();

    private:
        std::unordered_map<int32_t, SpawnerContext> _contexts;
    };

    inline auto EventObjectSpawnerComponent::GetSpawnDataRange()
    {
        return _contexts | std::views::values;
    }
}
