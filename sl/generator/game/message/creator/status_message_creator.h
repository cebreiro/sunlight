#pragma once
#include "sl/generator/game/contents/stat/hp_change_floater_type.h"
#include "sl/generator/game/contents/stat/sp_change_floater_type.h"

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

        static auto CreateHPChange(const GameEntity& entity, int32_t maxHP, int32_t hp, HPChangeFloaterType floater) -> Buffer;
        static auto CreateSPChange(const GameEntity& entity, int32_t maxSP, int32_t sp, SPChangeFloaterType floater) -> Buffer;
    };
}
