#pragma once

namespace sunlight
{
    struct DamageResult;
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

        static auto CreateDamageResult(const GameEntity& entity, const DamageResult& damageResult) -> Buffer;
    };
}
