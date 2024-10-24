#include "lua_player.h"

#include <sol/state.hpp>

#include "sl/emulator/game/contents/event_script/event_script.h"
#include "sl/emulator/game/contents/npc/npc_talk_box.h"
#include "sl/emulator/game/contents/quest/quest_change.h"
#include "sl/emulator/game/component/player_item_component.h"
#include "sl/emulator/game/component/player_job_component.h"
#include "sl/emulator/game/component/player_npc_script_component.h"
#include "sl/emulator/game/component/player_quest_component.h"
#include "sl/emulator/game/component/player_stat_component.h"
#include "sl/emulator/game/component/scene_object_component.h"
#include "sl/emulator/game/entity/game_monster.h"
#include "sl/emulator/game/entity/game_player.h"
#include "sl/emulator/game/script/class/lua_npc.h"
#include "sl/emulator/game/system/entity_view_range_system.h"
#include "sl/emulator/game/system/item_archive_system.h"
#include "sl/emulator/game/system/npc_shop_system.h"
#include "sl/emulator/game/system/player_quest_system.h"
#include "sl/emulator/game/system/player_state_system.h"
#include "sl/emulator/game/system/player_stat_system.h"
#include "sl/emulator/server/packet/io/sl_packet_writer.h"

namespace sunlight
{
    LuaPlayer::LuaPlayer(PlayerStateSystem& system, GamePlayer& player)
        : _system(system)
        , _player(player)
    {
    }

    bool LuaPlayer::IsMale() const
    {
        return _player.GetStatComponent().GetGender() == 0;
    }

    bool LuaPlayer::HasInventoryItem(int32_t itemId, int32_t quantity) const
    {
        return _player.GetItemComponent().HasInventoryItem(itemId, quantity);
    }

    void LuaPlayer::RecoverHP()
    {
        _system.Get<PlayerStatSystem>().RecoverHP(_player, HPChangeFloaterType::None);
    }

    void LuaPlayer::RecoverSP()
    {
        _system.Get<PlayerStatSystem>().RecoverSP(_player, SPChangeFloaterType::None);
    }

    bool LuaPlayer::AddItem(int32_t itemId, int32_t quantity)
    {
        return _system.Get<ItemArchiveSystem>().AddItem(_player, itemId, quantity);
    }

    bool LuaPlayer::RemoveInventoryItem(int32_t itemId, int32_t quantity)
    {
        return _system.Get<ItemArchiveSystem>().RemoveInventoryItem(_player, itemId, quantity);
    }

    void LuaPlayer::Show(const EventScript& eventScript)
    {
        _system.ShowEventScript(_player, eventScript);
    }

    void LuaPlayer::Talk(LuaNPC& npc, const NPCTalkBox& talkBox)
    {
        _system.CreateNPCTalkBox(_player, npc.GetImpl(), talkBox);
    }

    void LuaPlayer::DisposeTalk()
    {
        _system.DisposeNPCTalk(_player);
    }

    void LuaPlayer::OpenShop(LuaNPC& npc)
    {
        _system.Get<NPCShopSystem>().OpenItemShop(_player, npc.GetImpl());
    }

    void LuaPlayer::OpenAccountStorage()
    {
        _system.Get<ItemArchiveSystem>().OpenAccountStorage(_player);
    }

    void LuaPlayer::StartQuest(Quest quest)
    {
        _system.Get<PlayerQuestSystem>().StartQuest(_player, quest);
    }

    void LuaPlayer::ChangeQuest(int32_t questId, const QuestChange& change)
    {
        _system.Get<PlayerQuestSystem>().ChangeQuest(_player, questId, change);
    }

    auto LuaPlayer::FindQuest(int32_t quest) const -> const Quest*
    {
        return _player.GetQuestComponent().FindQuest(quest);
    }

    void LuaPlayer::ChangeStage(int32_t stageId, int32_t destX, int32_t destY)
    {
        _system.ChangeStage(_player, stageId, destX, destY);
    }

    void LuaPlayer::ChangeZone(int32_t zoneId, int32_t destX, int32_t destY)
    {
        _system.ChangeZone(_player, zoneId, destX, destY);
    }

    void LuaPlayer::Send(SlPacketWriter& writer)
    {
        _player.Send(writer.Flush());
    }

    void LuaPlayer::Broadcast(SlPacketWriter& writer, bool includeSelf)
    {
        _system.Get<EntityViewRangeSystem>().Broadcast(_player, writer.Flush(), includeSelf);
    }

    auto LuaPlayer::FindNearestMonster() const -> std::optional<LuaMonster>
    {
        GameMonster* result = nullptr;
        float distSq = std::numeric_limits<float>::max();

        Eigen::Vector2f playerPos = _player.GetSceneObjectComponent().GetPosition();

        _system.Get<EntityViewRangeSystem>().VisitMonster(_player, [&, playerPos](GameMonster& monster)
            {
                const float newDistSq = (monster.GetComponent<SceneObjectComponent>().GetPosition() - playerPos).squaredNorm();
                if (newDistSq < distSq)
                {
                    distSq = newDistSq;
                    result = &monster;
                }
            });

        return result ? LuaMonster(*result) : std::optional<LuaMonster>();
    }

    auto LuaPlayer::GetId() const -> int32_t
    {
        return static_cast<int32_t>(_player.GetId().Unwrap());
    }

    auto LuaPlayer::GetTypeValue() const -> int32_t
    {
        return static_cast<int32_t>(_player.GetType());
    }

    auto LuaPlayer::GetNoviceJobLevel() const -> int32_t
    {
        const Job* novice = _player.GetJobComponent().Find(JobType::Novice);

        return novice ? novice->GetLevel() : 0;
    }

    auto LuaPlayer::GetSelection() const -> int32_t
    {
        return _player.GetNPCScriptComponent().GetSelection();
    }

    auto LuaPlayer::GetState() const -> int32_t
    {
        return _player.GetNPCScriptComponent().GetState();
    }

    void LuaPlayer::SetState(int32_t state)
    {
        _player.GetNPCScriptComponent().SetState(state);
    }

    void LuaPlayer::Bind(sol::state& luaState)
    {
        luaState.new_usertype<LuaPlayer>("Player",
            sol::no_constructor,
            "isMale", &LuaPlayer::IsMale,
            "hasInventoryItem", &LuaPlayer::HasInventoryItem,
            "recoverHP", &LuaPlayer::RecoverHP,
            "recoverSP", &LuaPlayer::RecoverSP,
            "addItem", &LuaPlayer::AddItem,
            "removeInventoryItem", &LuaPlayer::RemoveInventoryItem,
            "show", &LuaPlayer::Show,
            "talk", &LuaPlayer::Talk,
            "disposeTalk", &LuaPlayer::DisposeTalk,
            "openShop", &LuaPlayer::OpenShop,
            "openAccountStorage", &LuaPlayer::OpenAccountStorage,
            "startQuest", &LuaPlayer::StartQuest,
            "changeQuest", &LuaPlayer::ChangeQuest,
            "findQuest", &LuaPlayer::FindQuest,
            "changeStage", &LuaPlayer::ChangeStage,
            "changeZone", &LuaPlayer::ChangeZone,
            "send", &LuaPlayer::Send,
            "broadcast", &LuaPlayer::Broadcast,
            "findNearestMonster", &LuaPlayer::FindNearestMonster,
            "getId", &LuaPlayer::GetId,
            "getTypeValue", &LuaPlayer::GetTypeValue,
            "getNoviceJobLevel", &LuaPlayer::GetNoviceJobLevel,
            "getSelection", &LuaPlayer::GetSelection,
            "getState", &LuaPlayer::GetState,
            "setState", &LuaPlayer::SetState
        );
    }

    auto LuaPlayer::GetCId() const -> int64_t
    {
        return _player.GetCId();
    }
}
