#pragma once
#include "sl/emulator/game/contants/quest/quest.h"

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

        bool AddItem(int32_t itemId, int32_t quantity);
        bool RemoveInventoryItem(int32_t itemId, int32_t quantity);

        void Show(const EventScript& eventScript);
        void Talk(LuaNPC& npc, const NPCTalkBox& talkBox);
        void DisposeTalk();
        void OpenShop(LuaNPC& npc);

        void StartQuest(Quest quest);
        void ChangeQuest(int32_t questId, const QuestChange& change);

        auto FindQuest(int32_t quest) const -> const Quest*;

        void Send(SlPacketWriter& writer);

        auto GetId() const -> int32_t;
        auto GetTypeValue() const -> int32_t;
        auto GetNoviceJobLevel() const -> int32_t;
        auto GetSelection() const -> int32_t;

    public:
        static void Bind(sol::state& luaState);

    public:
        auto GetCId() const -> int64_t;

    private:
        PlayerStateSystem& _system;
        GamePlayer& _player;
    };
}
