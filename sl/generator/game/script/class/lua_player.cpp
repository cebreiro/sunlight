#include "lua_player.h"

#include <sol/state.hpp>

#include "sl/generator/game/contents/event_script/event_script.h"
#include "sl/generator/game/contents/npc/npc_talk_box.h"
#include "sl/generator/game/contents/quest/quest_change.h"
#include "sl/generator/game/component/player_item_component.h"
#include "sl/generator/game/component/player_job_component.h"
#include "sl/generator/game/component/player_npc_script_component.h"
#include "sl/generator/game/component/player_quest_component.h"
#include "sl/generator/game/component/player_stat_component.h"
#include "sl/generator/game/component/scene_object_component.h"
#include "sl/generator/game/entity/game_monster.h"
#include "sl/generator/game/entity/game_player.h"
#include "sl/generator/game/script/class/lua_npc.h"
#include "sl/generator/game/system/entity_view_range_system.h"
#include "sl/generator/game/system/game_script_system.h"
#include "sl/generator/game/system/item_archive_system.h"
#include "sl/generator/game/system/npc_shop_system.h"
#include "sl/generator/game/system/player_job_system.h"
#include "sl/generator/game/system/player_quest_system.h"
#include "sl/generator/game/system/player_stat_system.h"
#include "sl/generator/server/packet/io/sl_packet_writer.h"

namespace sunlight
{
    LuaPlayer::LuaPlayer(GameScriptSystem& system, GamePlayer& player)
        : _system(system)
        , _player(player)
    {
    }

    bool LuaPlayer::IsMale() const
    {
        return _player.GetStatComponent().GetGender() == 0;
    }

    bool LuaPlayer::IsNovice() const
    {
        return _player.GetJobComponent().GetMainJob().IsNovice();
    }

    bool LuaPlayer::IsFighter() const
    {
        const Job* job = _player.GetJobComponent().Find(JobType::Novice);
        if (!job)
        {
            return false;
        }

        return job->GetId() == JobId::NoviceFighter;
    }

    bool LuaPlayer::IsRanger() const
    {
        const Job* job = _player.GetJobComponent().Find(JobType::Novice);
        if (!job)
        {
            return false;
        }

        return job->GetId() == JobId::NoviceRanger;

    }

    bool LuaPlayer::IsMagician() const
    {
        const Job* job = _player.GetJobComponent().Find(JobType::Novice);
        if (!job)
        {
            return false;
        }

        return job->GetId() == JobId::NoviceMagician;
    }

    bool LuaPlayer::IsArtisan() const
    {
        const Job* job = _player.GetJobComponent().Find(JobType::Novice);
        if (!job)
        {
            return false;
        }

        return job->GetId() == JobId::NoviceArtisan;
    }

    bool LuaPlayer::PromoteJob(int32_t jobId)
    {
        const JobId casted = static_cast<JobId>(jobId);
        if (!IsValid(casted))
        {
            return false;
        }

        return _system.Get<PlayerJobSystem>().Promote(_player, casted);
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

    bool LuaPlayer::Charge(int32_t cost)
    {
        return _system.Get<ItemArchiveSystem>().Charge(_player, cost);
    }

    bool LuaPlayer::AddItem(int32_t itemId, int32_t quantity)
    {
        return _system.Get<ItemArchiveSystem>().AddItem(_player, itemId, quantity);
    }

    bool LuaPlayer::RemoveInventoryItem(int32_t itemId, int32_t quantity)
    {
        return _system.Get<ItemArchiveSystem>().RemoveInventoryItem(_player, itemId, quantity);
    }

    void LuaPlayer::RemoveInventoryItemAll(int32_t itemId)
    {
        _system.Get<ItemArchiveSystem>().RemoveInventoryItemAll(_player, itemId);
    }

    void LuaPlayer::Show(const EventScript& eventScript)
    {
        _player.Show(eventScript);
    }

    void LuaPlayer::Talk(LuaNPC& npc, const NPCTalkBox& talkBox)
    {
        _system.Talk(_player, npc.GetImpl(), talkBox);
    }

    void LuaPlayer::DisposeTalk()
    {
        _system.DisposeTalk(_player);
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

    void LuaPlayer::ChangeZoneWithStage(int32_t zoneId, int32_t stage, int32_t destX, int32_t destY)
    {
        _system.ChangeZoneWithStage(_player, zoneId, stage, destX, destY);
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

    auto LuaPlayer::GetLevel() const -> int32_t
    {
        return _player.GetStatComponent().GetLevel();
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

    void LuaPlayer::SetSequence(int32_t sequence)
    {
        _player.GetNPCScriptComponent().SetSequence(sequence);
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
            "isNovice", &LuaPlayer::IsNovice,
            "isFighter", &LuaPlayer::IsFighter,
            "isRanger", &LuaPlayer::IsRanger,
            "isMagician", &LuaPlayer::IsMagician,
            "isArtisan", &LuaPlayer::IsArtisan,
            "hasInventoryItem", &LuaPlayer::HasInventoryItem,
            "promoteJob", &LuaPlayer::PromoteJob,
            "recoverHP", &LuaPlayer::RecoverHP,
            "recoverSP", &LuaPlayer::RecoverSP,
            "charge", &LuaPlayer::Charge,
            "addItem", &LuaPlayer::AddItem,
            "removeInventoryItem", &LuaPlayer::RemoveInventoryItem,
            "removeInventoryItemAll", &LuaPlayer::RemoveInventoryItemAll,
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
            "changeZoneWithStage", &LuaPlayer::ChangeZoneWithStage,
            "send", &LuaPlayer::Send,
            "broadcast", &LuaPlayer::Broadcast,
            "findNearestMonster", &LuaPlayer::FindNearestMonster,
            "getId", &LuaPlayer::GetId,
            "getTypeValue", &LuaPlayer::GetTypeValue,
            "getLevel", &LuaPlayer::GetLevel,
            "getNoviceJobLevel", &LuaPlayer::GetNoviceJobLevel,
            "getSelection", &LuaPlayer::GetSelection,
            "getState", &LuaPlayer::GetState,
            "setSequence", &LuaPlayer::SetSequence,
            "setState", &LuaPlayer::SetState
        );
    }

    auto LuaPlayer::GetCId() const -> int64_t
    {
        return _player.GetCId();
    }
}
