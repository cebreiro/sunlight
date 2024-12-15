#pragma once
#include "sl/generator/game/contents/group/street_vendor_start_result.h"
#include "sl/generator/game/contents/group/street_vendor_purchase_result.h"

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
        static auto CreateGroupCreationFail(int32_t groupId) -> Buffer;

        static auto CreateGroupShutdown(int32_t groupId) -> Buffer;

        static auto CreatePageItemDisplay(int32_t groupId, int32_t page, const GamePlayer& player) -> Buffer;
        static auto CreateItemPriceChange(int32_t groupId, int32_t page, int32_t price) -> Buffer;

        static auto CreateOpenResult(int32_t groupId, StreetVendorStartResult result) -> Buffer;
        static auto CreateItemPurchaseResult(int32_t groupId, StreetVendorPurchaseResult result) -> Buffer;

        static auto CreateVendorItemSoldOut(int32_t groupId) -> Buffer;
    };
}
