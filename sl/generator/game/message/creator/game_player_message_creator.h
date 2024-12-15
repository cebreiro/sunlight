#pragma once
#include "sl/generator/game/contents/group/game_group_type.h"
#include "sl/generator/game/contents/item/equipment_position.h"
#include "sl/generator/game/contents/job/job_id.h"
#include "sl/generator/game/contents/stat/hp_change_floater_type.h"
#include "sl/generator/game/contents/stat/sp_change_floater_type.h"

namespace sunlight
{
    class Quest;
    struct PlayerProfileIntroduction;
    struct GameGroupState;

    class GamePlayer;
    class GameItem;
}

namespace sunlight
{
    class GamePlayerMessageCreator
    {
    public:
        GamePlayerMessageCreator() = delete;

        static auto CreateAllState(const GamePlayer& player) -> Buffer;
        static auto CreateRemotePlayerState(const GamePlayer& player) -> Buffer;

        static auto CreateCharacterExpGain(const GamePlayer& player, int32_t value) -> Buffer;
        static auto CreateCharacterLevelUp(const GamePlayer& player) -> Buffer;
        static auto CreateJobExpGain(const GamePlayer& player, JobId jobId, int32_t exp) -> Buffer;
        static auto CreateJobExpLevelUp(const GamePlayer& player) -> Buffer;
        static auto CreateJobSkillAdd(const GamePlayer& player, JobId jobId, int32_t skillId, int32_t unk) -> Buffer;
        static auto CreateJobSkillLevelChange(const GamePlayer& player, int32_t skillId, int32_t skillLevel) -> Buffer;
        static auto CreateJobSkillPointChange(const GamePlayer& player, JobId jobId, int32_t newSkillPoint, bool unk) -> Buffer;
        static auto CreateJobPromotion(const GamePlayer& player, JobId jobId) -> Buffer;
        static auto CreateMixSkillExpChange(const GamePlayer& player, int32_t skillId, int32_t exp) -> Buffer;

        static auto CreatePlayerGainGroupItem(const GamePlayer& player, int32_t x, int32_t y) -> Buffer;

        static auto CreatePlayerWeaponSwap(const GamePlayer& player, const GameItem* weapon) -> Buffer;
        static auto CreateRemovePlayerWeaponChange(const GamePlayer& player) -> Buffer;

        static auto CreatePlayerEquipmentChange(const GamePlayer& player, EquipmentPosition position, int32_t modelId, int32_t modelColor) -> Buffer;
        static auto CreatePlayerHairColorChange(const GamePlayer& player, int32_t newColor) -> Buffer;
        static auto CreatePlayerHairChange(const GamePlayer& player, int32_t newHair, bool hasHat) -> Buffer;
        static auto CreatePlayerSkinColorChange(const GamePlayer& player, int32_t newColor) -> Buffer;

        static auto CreateQuestAllState(const GamePlayer& player) -> Buffer;
        static auto CreateQuestAdd(const GamePlayer& player, const Quest& quest) -> Buffer;
        static auto CreateQuestStateChange(const GamePlayer& player, int32_t questId, int32_t oldState, int32_t newState) -> Buffer;
        static auto CreateQuestFlagSet(const GamePlayer& player, int32_t questId, int32_t state, int32_t index, int32_t flag) -> Buffer;
        static auto CreateQuestGiveUpNotify(const GamePlayer& player, bool success) -> Buffer;

        static auto CreateEventScriptAddString(const GamePlayer& player, int32_t index) -> Buffer;
        static auto CreateEventScriptAddStringWithInt(const GamePlayer& player, int32_t index, int32_t value) -> Buffer;
        static auto CreateEventScriptAddStringWithItem(const GamePlayer& player, int32_t index, int32_t itemId) -> Buffer;
        static auto CreateEventScriptClear(const GamePlayer& player) -> Buffer;
        static auto CreateEventScriptShow(const GamePlayer& player) -> Buffer;

        static auto CreateProfileSettingChangeResult(const GamePlayer& player) -> Buffer;
        static auto CreateProfileIntroduction(const GamePlayer& player, const PlayerProfileIntroduction& introduction) -> Buffer;
        static auto CreateProfileIntroductionFail(const GamePlayer& player) -> Buffer;
        static auto CreateProfileIntroductionSaveResult(const GamePlayer& player) -> Buffer;

        static auto CreatePlayerStateProposition(const GamePlayer& player, const std::string& title, int32_t groupId, GameGroupType groupType, int32_t unk2) -> Buffer;
        static auto CreatePlayerStateProposition(const GamePlayer& player, const GameGroupState& groupState) -> Buffer;
    };
}
