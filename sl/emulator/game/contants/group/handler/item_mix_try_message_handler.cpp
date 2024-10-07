#include "item_mix_try_message_handler.h"

#include <boost/container/small_vector.hpp>

#include "sl/emulator/game/component/player_item_component.h"
#include "sl/emulator/game/component/player_skill_component.h"
#include "sl/emulator/game/contants/group/game_group.h"
#include "sl/emulator/game/entity/game_item.h"
#include "sl/emulator/game/entity/game_player.h"
#include "sl/emulator/game/message/creator/item_mix_message_creator.h"
#include "sl/emulator/game/system/player_group_system.h"
#include "sl/emulator/server/packet/io/sl_packet_reader.h"
#include "sl/emulator/service/gamedata/gamedata_provide_service.h"
#include "sl/emulator/service/gamedata/item/item_data.h"
#include "sl/emulator/service/gamedata/item_mix/item_mix_data_provider.h"

namespace sunlight
{
    void ItemMixTryMessageHandler::Handle(PlayerGroupSystem& system, GameGroup& group, GamePlayer& player, SlPacketReader& reader) const
    {
        const auto& serviceLocator = system.GetServiceLocator();

        const int32_t itemId = reader.Read<int32_t>();
        const int32_t skillId = reader.Read<int32_t>();

        [[maybe_unused]]
        const auto [mixToolItemId, unused] = reader.ReadInt64();

        bool result = false;
        std::string log;

        do
        {
            PlayerSkillComponent& skillComponent = player.GetSkillComponent();
            const PlayerSkill* skill = skillComponent.FindSkill(skillId);

            if (!skill)
            {
                log = fmt::format("fail to find ski. player: {}, skill: {}", player.GetCId(), skillId);

                break;
            }

            const ItemMixDataProvider& itemMixDataProvider = serviceLocator.Get<GameDataProvideService>().GetItemMixDataProvider();
            const ItemMixGroupMemberData* itemMixData = itemMixDataProvider.Find(skillId)->Find(skill->GetLevel(), itemId);

            if (!itemMixData)
            {
                log = fmt::format("fail to find item mix data. player: {}, skill {}, skill_level: {}, item_id: {}",
                    player.GetCId(), skillId, skill->GetLevel(), itemId);

                break;
            }

            int32_t materialLevel = 0;

            const std::vector<ItemMixMaterial>& dataMaterials = itemMixData->GetMaterials();
            boost::container::small_vector<ItemMixMaterial, 4> materials(dataMaterials.begin(), dataMaterials.end());

            constexpr int32_t itemIdGradeRuleConstant = 10000000;

            for (const GameItem& mixItem : player.GetItemComponent().GetMixItems())
            {
                const int32_t mixItemId = mixItem.GetData().GetId();
                const auto iter = std::ranges::find_if(materials, [mixItemId](const ItemMixMaterial& material) -> bool
                    {
                        if (material.quantity <= 0)
                        {
                            return false;
                        }

                        return (mixItemId % itemIdGradeRuleConstant) == (material.itemId % itemIdGradeRuleConstant);
                    });
                if (iter != materials.end())
                {
                    iter->quantity -= mixItem.GetQuantity();

                    materialLevel += std::max(0, (mixItemId / itemIdGradeRuleConstant) - 1);
                }
            }

            if (std::ranges::any_of(materials, [](const ItemMixMaterial& material) -> bool
                {
                    return material.quantity > 0;
                }))
            {
                log = fmt::format("lack of mix item material. player: {}, skill {}, skill_level: {}, item_id: {}",
                    player.GetCId(), skillId, skill->GetLevel(), itemId);

                break;
            }

            const int32_t gradeLevel = skill->GetLevel() + materialLevel;
            const std::array<int32_t, item_mix_grade_weight_size>* weightData = itemMixDataProvider.FindWeight(itemMixData->GetGradeType(), gradeLevel);

            if (!weightData)
            {
                log = fmt::format("fail to find weight data. player: {}, skill {}, skill_level: {}, item_id: {}, grade_type: {}grade_level: {}",
                    player.GetCId(), skillId, skill->GetLevel(), itemId, itemMixData->GetGradeType(), gradeLevel);

                break;
            }

            int32_t prev = 0;
            boost::container::small_vector<std::pair<ItemMixGradeWeightType, int32_t>, item_mix_grade_weight_size> weights;

            for (int32_t i = 0; i < item_mix_grade_weight_size; ++i)
            {
                const int32_t value = (*weightData)[i];
                if (value != 0)
                {
                    const int32_t weight = prev + value;
                    weights.emplace_back(static_cast<ItemMixGradeWeightType>(i), weight);

                    prev = weight;
                }
            }

            assert(!weights.empty());

            const ItemMixGradeWeightType grade = [&]() -> ItemMixGradeWeightType
                {
                    std::uniform_int_distribution dist(0, weights.back().second);

                    const int32_t rand = dist(system.GetRandomEngine());

                    for (const auto& [type, value] : weights)
                    {
                        if (rand <= value)
                        {
                            return type;
                        }
                    }

                    assert(false);

                    return weights.begin()->first;
                }();

            const ItemData* itemData = [&]() -> const ItemData*
                {
                    const std::array<const ItemData*, item_mix_grade_count>& dataList = itemMixData->GetResultItemDataList();

                    switch (grade)
                    {
                    case ItemMixGradeWeightType::Low:
                        return dataList[static_cast<int32_t>(ItemMixGradeType::Low)];
                    case ItemMixGradeWeightType::Middle:
                        return dataList[static_cast<int32_t>(ItemMixGradeType::Middle)];
                    case ItemMixGradeWeightType::High:
                        return dataList[static_cast<int32_t>(ItemMixGradeType::High)];
                    case ItemMixGradeWeightType::Super:
                        return dataList[static_cast<int32_t>(ItemMixGradeType::Super)];
                    case ItemMixGradeWeightType::Fail:
                    case ItemMixGradeWeightType::Count:
                    default:;
                    }

                    return nullptr;
                }();

            if (itemData)
            {
                player.Send(ItemMixMessageCreator::CreateItemMixSuccess(group.GetId(), itemData->GetId(), 1, 0));
            }
            else
            {
                player.Send(ItemMixMessageCreator::CreateItemMixFailure(group.GetId(), 1, 0));
            }

            result = true;

        } while (false);

        if (!result)
        {
            SUNLIGHT_LOG_ERROR(serviceLocator, log);

            player.Send(ItemMixMessageCreator::CreateItemMixSuccess(group.GetId(), itemId, 1, 0));
        }
    }
}
