#pragma once

namespace sunlight
{
    class StatusEffect;

    class GameEntity;
}

namespace sunlight
{
    class StatusMessageCreator
    {
    public:
        StatusMessageCreator() = delete;

        static auto CreateStatusEffectAdd(const GameEntity& entity, const StatusEffect& effect) -> Buffer;
    };
}
