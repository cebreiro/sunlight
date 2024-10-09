#pragma once
#include "sl/emulator/game/entity/game_entity_id_type.h"
#include "sl/emulator/game/script/class/lua_player.h"
#include "sl/emulator/game/system/game_system.h"

namespace sunlight
{
    struct MapStage;
    struct ZoneMessage;

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
        PlayerStateSystem(const ServiceLocator& serviceLocator, const MapStage& stageData);

        void InitializeSubSystem(Stage& stage) override;
        bool Subscribe(Stage& stage) override;
        auto GetName() const -> std::string_view override;
        auto GetClassId() const -> game_system_id_type override;

        auto GetServiceLocator() const -> const ServiceLocator&;

    public:
        void ShowEventScript(GamePlayer& player, const EventScript& eventScript);
        void CreateNPCTalkBox(GamePlayer& player, GameNPC& npc, const NPCTalkBox& talkBox);
        void DisposeNPCTalk(GamePlayer& player);

        void ChangeStage(GamePlayer& player, int32_t stageId, int32_t destX, int32_t destY);
        void ChangeZone(GamePlayer& player, int32_t zoneId, int32_t destX, int32_t destY);

    public:
        void OnUseItem(const ZoneMessage& message);

    private:
        void HandleNPCConversation(GamePlayer& player, game_entity_id_type target);
        void HandlePlayerInteraction(GamePlayer& player, game_entity_id_type target);
        void HandleScriptState(const ZoneMessage& message);

    private:
        bool HandleCharacterState(const ZoneMessage& message);

        void HandlePickGroundItem(GamePlayer& player, game_entity_id_type itemId);

    private:
        const ServiceLocator& _serviceLocator;
        const MapStage& _stageData;
    };
}
