#pragma once
#include "sl/emulator/game/contants/item/equipment_position.h"
#include "sl/emulator/game/contants/job/job_id.h"

namespace sunlight
{
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
        static auto CreateJobSkillPointChange(const GamePlayer& player, JobId jobId, int32_t newSkillPoint, bool unk) -> Buffer;
        static auto CreateJobPromotion(const GamePlayer& player, JobId jobId) -> Buffer;

        static auto CreatePlayerGainGroupItem(const GamePlayer& player, int32_t x, int32_t y) -> Buffer;

        static auto CreatePlayerWeaponSwap(const GamePlayer& player, const GameItem* weapon) -> Buffer;
        static auto CreateRemovePlayerWeaponChange(const GamePlayer& player) -> Buffer;

        static auto CreatePlayerEquipmentChange(const GamePlayer& player, EquipmentPosition position, int32_t modelId, int32_t modelColor) -> Buffer;
        static auto CreatePlayerHairColorChange(const GamePlayer& player, int32_t newColor) -> Buffer;
        static auto CreatePlayerSkinColorChange(const GamePlayer& player, int32_t newColor) -> Buffer;
    };
}
