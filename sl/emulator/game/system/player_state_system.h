#pragma once
#include "sl/emulator/game/entity/game_entity_id_type.h"
#include "sl/emulator/game/script/class/lua_player.h"
#include "sl/emulator/game/system/game_system.h"

namespace sunlight
{
    struct MapStage;
    struct ZoneMessage;
    struct GameEntityState;

    class NPCTalkBox;
    class EventScript;

    class GamePlayer;
    class GameNPC;
}

namespace sunlight
{
    class PlayerStateSystem final : public GameSystem
    {
    public:
        PlayerStateSystem(const ServiceLocator& serviceLocator, const MapStage& stageData, int32_t zoneId);
        ~PlayerStateSystem();

        void InitializeSubSystem(Stage& stage) override;
        bool Subscribe(Stage& stage) override;
        auto GetName() const -> std::string_view override;
        auto GetClassId() const -> game_system_id_type override;

        auto GetServiceLocator() const -> const ServiceLocator&;

    public:
        void OnUseItem(const ZoneMessage& message);

    private:
        void HandleNPCConversation(GamePlayer& player, game_entity_id_type target);
        void HandlePlayerInteraction(GamePlayer& player, game_entity_id_type target);
        void HandleScriptState(const ZoneMessage& message);

    private:
        bool HandleCharacterState(const ZoneMessage& message);

        void HandlePickGroundItem(GamePlayer& player, game_entity_id_type itemId);
        void HandlePlayerSkill(GamePlayer& player, const GameEntityState& state);
        void HandleNormalAttack(GamePlayer& player, const GameEntityState& state);

    private:
        static bool GetRevivalPoint(int32_t zoneId, int32_t& destZone, Eigen::Vector2f& destPos);

    private:
        const ServiceLocator& _serviceLocator;
        const MapStage& _stageData;
        int32_t _zoneId = 0;
    };
}
