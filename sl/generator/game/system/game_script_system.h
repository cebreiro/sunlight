#pragma once
#include "sl/generator/game/system/game_system.h"

namespace sunlight
{
    class NPCTalkBox;

    class GameNPC;
    class GamePlayer;
}

namespace sunlight
{
    class GameScriptSystem final : public GameSystem
    {
    public:
        GameScriptSystem(const ServiceLocator& serviceLocator, int32_t stageId);

        void InitializeSubSystem(Stage& stage) override;
        bool Subscribe(Stage& stage) override;
        auto GetName() const -> std::string_view override;
        auto GetClassId() const -> game_system_id_type override;

        auto GetServiceLocator() const -> const ServiceLocator&;

    public:
        bool StartNPCScript(GamePlayer& player, GameNPC& npc);
        void ContinueNPCScript(GamePlayer& player, int32_t selection);

        void StartQuestScriptMonsterKill(GamePlayer& player, int32_t questId, int32_t monsterId);

    public:
        void Talk(GamePlayer& player, GameNPC& npc, const NPCTalkBox& talkBox);
        void DisposeTalk(GamePlayer& player);

        void ChangeStage(GamePlayer& player, int32_t stageId, int32_t destX, int32_t destY);
        void ChangeZone(GamePlayer& player, int32_t zoneId, int32_t destX, int32_t destY);
        void ChangeZoneWithStage(GamePlayer& player, int32_t zoneId, int32_t stage, int32_t destX, int32_t destY);

    private:
        const ServiceLocator& _serviceLocator;

        int32_t _stageId = 0;
    };
}
