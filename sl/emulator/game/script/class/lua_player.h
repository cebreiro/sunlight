#pragma once
#include "sl/emulator/game/contents/quest/quest.h"
#include "sl/emulator/game/script/class/lua_monster.h"

namespace sol
{
    class state;
}

namespace sunlight
{
    class PlayerStateSystem;
    class GamePlayer;
    class QuestChange;

    class LuaNPC;
    class NPCTalkBox;
    class EventScript;

    class SlPacketWriter;
}

namespace sunlight
{
    class LuaPlayer
    {
    public:
        LuaPlayer(PlayerStateSystem& system, GamePlayer& player);

        bool IsMale() const;
        bool HasInventoryItem(int32_t itemId, int32_t quantity) const;

        void RecoverHP();
        void RecoverSP();

        bool AddItem(int32_t itemId, int32_t quantity);
        bool RemoveInventoryItem(int32_t itemId, int32_t quantity);

        void Show(const EventScript& eventScript);
        void Talk(LuaNPC& npc, const NPCTalkBox& talkBox);
        void DisposeTalk();
        void OpenShop(LuaNPC& npc);
        void OpenAccountStorage();

        void StartQuest(Quest quest);
        void ChangeQuest(int32_t questId, const QuestChange& change);

        auto FindQuest(int32_t quest) const -> const Quest*;

        void ChangeStage(int32_t stageId, int32_t destX, int32_t destY);
        void ChangeZone(int32_t zoneId, int32_t destX, int32_t destY);

        void Send(SlPacketWriter& writer);
        void Broadcast(SlPacketWriter& writer, bool includeSelf);

        auto FindNearestMonster() const -> std::optional<LuaMonster>;

        auto GetId() const -> int32_t;
        auto GetTypeValue() const -> int32_t;
        auto GetNoviceJobLevel() const -> int32_t;
        auto GetSelection() const -> int32_t;
        auto GetState() const -> int32_t;

        void SetState(int32_t state);

    public:
        static void Bind(sol::state& luaState);

    public:
        auto GetCId() const -> int64_t;

    private:
        PlayerStateSystem& _system;
        GamePlayer& _player;
    };
}
