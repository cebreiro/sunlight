#pragma once
#include "sl/emulator/game/contents/group/group_message_handler_interface.h"

namespace sunlight
{
    class StreetVendorChangePriceMessageHandler final : public IGroupMessageHandler
    {
    public:
        void Handle(PlayerGroupSystem& system, GameGroup& group, GamePlayer& player, SlPacketReader& reader) const override;
    };
}
