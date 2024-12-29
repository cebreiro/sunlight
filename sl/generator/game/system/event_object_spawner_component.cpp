#include "event_object_spawner_component.h"

#include "sl/generator/service/gamedata/monster/monster_data.h"

namespace sunlight
{
    void EventObjectSpawnerComponent::AddContext(const MonsterData& data, int32_t maxCount, int32_t firstDelay, int32_t delay)
    {
        const int32_t id = data.GetId();

        assert(!_contexts.contains(id));

        SpawnerContext& context = _contexts[id];
        context.data = &data;
        context.maxCount = maxCount;
        context.firstDelay = firstDelay;
        context.delay = delay;
    }

    auto EventObjectSpawnerComponent::FindContext(int32_t mobId) -> SpawnerContext*
    {
        const auto iter = _contexts.find(mobId);

        return iter != _contexts.end() ? &iter->second : nullptr;
    }

    auto EventObjectSpawnerComponent::FindContext(int32_t mobId) const -> const SpawnerContext*
    {
        const auto iter = _contexts.find(mobId);

        return iter != _contexts.end() ? &iter->second : nullptr;
    }
}
