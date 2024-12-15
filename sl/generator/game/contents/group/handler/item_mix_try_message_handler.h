#pragma once
#include <boost/container/small_vector.hpp>
#include "sl/generator/game/contents/group/group_message_handler_interface.h"
#include "sl/generator/game/contents/group/item_mix_material.h"

namespace sunlight
{
    class ItemData;
    class ItemMixData;
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

        auto RollDiceAndGetResult(PlayerGroupSystem& system, const ItemMixDataProvider& dataProvider, const ItemMixGroupMemberData& data, int32_t gradeLevel) const
            -> const ItemData*;

        static auto CalculateItemMixExp(const ItemMixDataProvider& dataProvider, const ItemMixGroupMemberData& memberData, int32_t skillLevel, int32_t materialLevel) -> int32_t;
    };
}
