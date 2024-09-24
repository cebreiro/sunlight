#pragma once
#include "sl/emulator/game/entity/game_entity_id_type.h"
#include "sl/emulator/game/script/class/lua_player.h"
#include "sl/emulator/game/system/game_system.h"

namespace sunlight
{
    struct ZoneMessage;

    class NPCTalkBox;

    class GamePlayer;
    class GameNPC;
}

namespace sunlight
{
    class PlayerStateSystem final : public GameSystem
    {
    public:
        explicit PlayerStateSystem(const ServiceLocator& serviceLocator);

        void InitializeSubSystem(Stage& stage) override;
        bool Subscribe(Stage& stage) override;
        auto GetName() const -> std::string_view override;
        auto GetClassId() const -> game_system_id_type override;

        auto GetServiceLocator() const -> const ServiceLocator&;

    public:
        void CreateNPCTalkBox(GamePlayer& player, GameNPC& npc, const NPCTalkBox& talkBox);
        void DisposeNPCTalk(GamePlayer& player);

    private:
        void StartNPCScript(GamePlayer& player, game_entity_id_type target);
        void HandleScriptState(const ZoneMessage& message);

    private:
        bool HandleCharacterState(const ZoneMessage& message);

        void HandlePickGroundItem(GamePlayer& player, game_entity_id_type itemId);

    private:
        const ServiceLocator& _serviceLocator;
    };
}
