#pragma once
#include "sl/generator/game/contents/group/item_mix_window_open_result.h"

namespace sunlight
{
    class GamePlayer;
    class GameItem;
}

namespace sunlight
{
    class ItemMixMessageCreator
    {
    public:
        ItemMixMessageCreator() = delete;

        static auto CreateItemMixWindowByPropSuccess(const GamePlayer& player, int32_t itemId) -> Buffer;
        static auto CreateItemMixWindowByPropFail(const GamePlayer& player) -> Buffer;
        static auto CreateItemMixWindowByItemSuccess(const GamePlayer& player, const GameItem& item) -> Buffer;
        static auto CreateItemMixWindowByItemFail(const GamePlayer& player, ItemMixOpenByItemFailReason result) -> Buffer;

        static auto CreateGroupCreate(int32_t groupId) -> Buffer;

        static auto CreateItemLowerResult(int32_t groupId) -> Buffer;
        static auto CreateItemMixSuccess(int32_t groupId, int32_t itemId, int32_t mixSkillLevel, int32_t mixSkillExp) -> Buffer;
        static auto CreateItemMixFailure(int32_t groupId, int32_t mixSkillLevel, int32_t mixSkillExp) -> Buffer;
    };
}
