#pragma once

namespace sunlight
{
    struct AttackResult;
    class StatusEffect;

    class GameEntity;
}

namespace sunlight
{
    class StatusMessageCreator
    {
    public:
        StatusMessageCreator() = delete;

        static auto CreateStatusEffectAdd(const GameEntity& entity, const StatusEffect& statusEffect) -> Buffer;
        static auto CreateStatusEffectRemove(const GameEntity& entity, const StatusEffect& statusEffect) -> Buffer;

        static auto CreateAttackResult(const GameEntity& entity, const AttackResult& attackResult) -> Buffer;
    };
}
