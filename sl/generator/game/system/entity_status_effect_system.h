#pragma once
#include <boost/unordered/unordered_flat_map.hpp>
#include "sl/generator/game/contents/status_effect/status_effect_handler_interface.h"
#include "sl/generator/game/entity/game_entity_id_type.h"
#include "sl/generator/game/system/game_system.h"
#include "sl/generator/game/zone/stage_enter_type.h"
#include "sl/generator/game/zone/stage_exit_type.h"

namespace sunlight
{
    struct SkillEffectData;

    class GameEntity;
    class GamePlayer;
    class GameMonster;
}

namespace sunlight
{
    class EntityStatusEffectSystem final : public GameSystem
    {
    public:
        EntityStatusEffectSystem(const ServiceLocator& serviceLocator, int32_t stageId);

        void InitializeSubSystem(Stage& stage) override;
        bool Subscribe(Stage& stage) override;
        bool ShouldUpdate() const override;
        void Update() override;
        auto GetName() const -> std::string_view override;
        auto GetClassId() const -> game_system_id_type override;

    public:
        void OnStageEnter(GamePlayer& player, StageEnterType enterType);
        void OnStageExit(GamePlayer& player, StageExitType exitType);

        void OnStageExit(GameMonster& monster);

    public:
        void AddStatusEffectBySkill(int32_t skillId, int32_t skillLevel,
            std::span<game_entity_id_type> targets, const SkillEffectData& skillEffectData);
        void AddStatusEffectByPassive(int32_t skillId, int32_t skillLevel, GameEntity& target, const SkillEffectData& skillEffectData);

        bool RemoveStatusEffect(GameEntity& entity, int32_t statusEffectId);

    public:
        template <typename T> requires std::derived_from<T, IStatusEffectHandler>
        void AddHandler(UniquePtrNotNull<T> handler);

    private:
        void ClearStatusEffect(GameEntity& entity, StatusEffect& statusEffect);
        void AddStatusEffectRemoveTimer(const GameEntity& entity, const StatusEffect& statusEffect);

    private:
        auto GetApplyHandler(StatusEffectType type) const -> const IStatusEffectApplyHandler*;
        auto GetRevertHandler(StatusEffectType type) const -> const IStatusEffectRevertHandler*;
        auto GetTickHandler(StatusEffectType type) const -> const IStatusEffectTickHandler*;

    private:
        const ServiceLocator& _serviceLocator;
        int32_t _stageId = 0;

        boost::unordered::unordered_flat_map<PtrNotNull<GameEntity>, std::vector<PtrNotNull<StatusEffect>>> _tickStatusEffects;

        std::unordered_map<StatusEffectType, UniquePtrNotNull<IStatusEffectHandler>> _handlers;
        std::unordered_map<StatusEffectType, PtrNotNull<const IStatusEffectApplyHandler>> _applyHandlers;
        std::unordered_map<StatusEffectType, PtrNotNull<const IStatusEffectRevertHandler>> _revertHandlers;
        std::unordered_map<StatusEffectType, PtrNotNull<const IStatusEffectTickHandler>> _tickHandlers;
    };

    template <typename T> requires std::derived_from<T, IStatusEffectHandler>
    void EntityStatusEffectSystem::AddHandler(UniquePtrNotNull<T> handler)
    {
        const StatusEffectType type = handler->GetType();
        const T* ptr = handler.get();

        [[maybe_unused]]
        const bool inserted = _handlers.try_emplace(type, std::move(handler)).second;
        assert(inserted);

        if constexpr (std::derived_from<T, IStatusEffectApplyHandler>)
        {
            _applyHandlers[type] = ptr;
        }

        if constexpr (std::derived_from<T, IStatusEffectApplyHandler>)
        {
            _revertHandlers[type] = ptr;
        }

        if constexpr (std::derived_from<T, IStatusEffectTickHandler>)
        {
            _tickHandlers[type] = ptr;
        }
    }
}
