#pragma once
#include <boost/container/small_vector.hpp>
#include "sl/emulator/game/contants/group/group_message_handler_interface.h"
#include "sl/emulator/game/contants/group/item_mix_material.h"

namespace sunlight
{
    class ItemData;
    class ItemMixDataProvider;
    class ItemMixGroupMemberData;

    class GamePlayer;
}

namespace sunlight
{
    class ItemMixTryMessageHandler final : public IGroupMessageHandler
    {
    public:
        void Handle(PlayerGroupSystem& system, GameGroup& group, GamePlayer& player, SlPacketReader& reader) const override;

    private:
        bool GetMaterial(boost::container::small_vector<ItemMixMaterial, 4>& materials, int32_t& materialLevel,
            const GamePlayer& player, const ItemMixGroupMemberData& data) const;

        auto GetResultItem(PlayerGroupSystem& system, const ItemMixDataProvider& dataProvider, const ItemMixGroupMemberData& data, int32_t gradeLevel) const
            -> const ItemData*;
    };
}
