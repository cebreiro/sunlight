#pragma once
#include "sl/emulator/game/contants/group/street_vendor_start_result.h"

namespace sunlight
{
    class GamePlayer;
}
namespace sunlight
{
    class StreetVendorMessageCreator
    {
    public:
        StreetVendorMessageCreator() = delete;

        static auto CreateGroupCreate(int32_t groupId) -> Buffer;

        static auto CreatePageItemDisplay(int32_t groupId, int32_t maxPage, const GamePlayer& player) -> Buffer;
        static auto CreateItemPriceChange(int32_t groupId, int32_t maxPage, int32_t price) -> Buffer;

        static auto CreateOpenResult(int32_t groupId, StreetVendorStartResult result) -> Buffer;
    };
}
