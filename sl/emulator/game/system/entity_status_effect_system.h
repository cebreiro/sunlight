#pragma once
#include "sl/emulator/game/contents/status_effect/status_effect_handler_interface.h"
#include "sl/emulator/game/system/game_system.h"
#include "sl/emulator/service/gamedata/skill/player_skill_effect_data.h"

namespace sunlight
{
    class GameEntity;
}

namespace sunlight
{
    class EntityStatusEffectSystem final : public GameSystem
    {
    public:
        explicit EntityStatusEffectSystem(const ServiceLocator& serviceLocator);

        void InitializeSubSystem(Stage& stage) override;
        bool Subscribe(Stage& stage) override;
        bool ShouldUpdate() const override;
        void Update() override;
        auto GetName() const -> std::string_view override;
        auto GetClassId() const -> game_system_id_type override;

    public:
        template <typename T> requires std::derived_from<T, IStatusEffectHandler>
        void AddHandler(UniquePtrNotNull<T> handler);

    public:
        void AddStatusEffect(int32_t skillId, int32_t skillLevel,
            std::span<PtrNotNull<GameEntity>> targets,
            std::span<const SkillEffectStatusEffect> statusEffectDataList);


    

    private:
        const ServiceLocator& _serviceLocator;

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
