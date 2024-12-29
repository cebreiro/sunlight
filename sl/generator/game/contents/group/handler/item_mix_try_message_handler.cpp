#include "item_mix_try_message_handler.h"

#include <boost/container/small_vector.hpp>

#include "sl/generator/game/component/player_group_component.h"
#include "sl/generator/game/component/player_item_component.h"
#include "sl/generator/game/component/player_skill_component.h"
#include "sl/generator/game/contents/group/game_group.h"
#include "sl/generator/game/entity/game_item.h"
#include "sl/generator/game/entity/game_player.h"
#include "sl/generator/game/message/creator/game_player_message_creator.h"
#include "sl/generator/game/message/creator/item_mix_message_creator.h"
#include "sl/generator/game/system/item_archive_system.h"
#include "sl/generator/game/system/player_group_system.h"
#include "sl/generator/game/zone/service/game_repository_service.h"
#include "sl/generator/game/zone/service/zone_execution_service.h"
#include "sl/generator/server/packet/io/sl_packet_reader.h"
#include "sl/generator/service/gamedata/gamedata_provide_service.h"
#include "sl/generator/service/gamedata/item/item_data.h"
#include "sl/generator/service/gamedata/item_mix/item_mix_data_provider.h"

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
            PlayerSkill* skill = skillComponent.FindSkill(skillId);

            if (!skill)
            {
                log = fmt::format("fail to find ski. player: {}, skill: {}", player.GetCId(), skillId);

                break;
            }

            const ItemMixDataProvider& itemMixDataProvider = serviceLocator.Get<GameDataProvideService>().GetItemMixDataProvider();
            const ItemMixData* mixData = itemMixDataProvider.Find(skillId);
            const ItemMixGroupMemberData* mixMemberData = mixData ? mixData->Find(skill->GetLevel(), itemId) : nullptr;

            if (!mixMemberData)
            {
                log = fmt::format("fail to find item mix data. player: {}, skill {}, skill_level: {}, item_id: {}",
                    player.GetCId(), skillId, skill->GetLevel(), itemId);

                break;
            }

            boost::container::small_vector<ItemMixMaterial, 4> mixMaterials;
            int32_t materialLevel = 0;

            if (!GetMaterial(mixMaterials, materialLevel, player, *mixMemberData))
            {
                log = fmt::format("fail to gather player mix items. player: {}, skill {}, skill_level: {}, item_id: {}, grade_type: {}",
                    player.GetCId(), skillId, skill->GetLevel(), itemId, mixMemberData->GetGradeType());
            }

            const int32_t gradeLevel = skill->GetLevel() + materialLevel;
            const ItemData* itemData = RollDiceAndGetResult(system, itemMixDataProvider, *mixMemberData, gradeLevel);

            const int32_t mixTime = mixMemberData->GetMixTimeMilli();

            const auto processMix = [&system, itemData, mixData, mixMemberData, mixMaterials, materialLevel, skill, &itemMixDataProvider](GamePlayer& player, GameGroup& group)
                {
                    if (itemData)
                    {
                        system.Get<ItemArchiveSystem>().OnItemMixSuccess(player, *itemData, mixMemberData->GetResultItemCount(), mixMaterials);

                        int32_t level = skill->GetLevel();
                        int32_t exp = skill->GetEXP();

                        if (skill->GetLevel() < 30)
                        {
                            if (const std::optional<int32_t> requiredExp = itemMixDataProvider.GetLevelUpExp(mixData->GetDifficultyType(), level);
                                requiredExp.has_value())
                            {
                                const int32_t addExp = CalculateItemMixExp(itemMixDataProvider, *mixMemberData, level, materialLevel);

                                exp += addExp;
                                if (exp >= *requiredExp)
                                {
                                    ++level;

                                    player.Defer(GamePlayerMessageCreator::CreateJobSkillLevelChange(player, skill->GetId(), level));
                                }
                            }

                            skill->SetBaseLevel(level);
                            skill->SetEXP(exp);

                            player.Defer(GamePlayerMessageCreator::CreateMixSkillExpChange(player, skill->GetId(), exp));
                        }

                        system.GetServiceLocator().Get<GameRepositoryService>().SaveMixSkillExp(player, skill->GetId(), level, exp);

                        player.Defer(ItemMixMessageCreator::CreateItemMixSuccess(group.GetId(), itemData->GetId(), level, exp));
                    }
                    else
                    {
                        system.Get<ItemArchiveSystem>().OnItemMixFail(player, mixMaterials);

                        player.Defer(ItemMixMessageCreator::CreateItemMixFailure(group.GetId(), skill->GetLevel(), skill->GetEXP()));
                    }

                    player.FlushDeferred();
                };

            if (mixTime <= 0)
            {
                processMix(player, group);
            }
            else
            {
                system.GetServiceLocator().Get<ZoneExecutionService>().AddTimer(std::chrono::milliseconds(mixTime),
                    player.GetCId(), system.GetStageId(), [&system, groupId = group.GetId(), processMix](GamePlayer& player)
                    {
                        PlayerGroupComponent& groupComponent = player.GetGroupComponent();

                        if (!groupComponent.HasGroup())
                        {
                            return;
                        }

                        if (groupComponent.GetGroupId() != groupId)
                        {
                            return;
                        }

                        GameGroup* group = system.FindGroup(groupId);
                        if (!group)
                        {
                            return;
                        }

                        processMix(player, *group);
                    });
            }

            result = true;

        } while (false);

        if (!result)
        {
            SUNLIGHT_LOG_ERROR(serviceLocator, log);

            player.Send(ItemMixMessageCreator::CreateItemMixSuccess(group.GetId(), itemId, 1, 0));
        }
    }

    bool ItemMixTryMessageHandler::GetMaterial(boost::container::small_vector<ItemMixMaterial, 4>& materials,
        int32_t& materialLevel, const GamePlayer& player, const ItemMixGroupMemberData& data) const
    {
        const std::vector<ItemMixMaterialData>& dataMaterials = data.GetMaterials();
        auto mixItems = player.GetItemComponent().GetMixItems();

        constexpr int32_t itemIdGradeRuleConstant = 10000000;

        for (const ItemMixMaterialData& material : dataMaterials)
        {
            const int32_t id = material.itemId % itemIdGradeRuleConstant;

            const auto iter = std::ranges::find_if(mixItems, [id](const GameItem& mixItem) -> bool
                {
                    return id == (mixItem.GetData().GetId() % itemIdGradeRuleConstant);
                });

            if (iter == mixItems.end())
            {
                return false;
            }

            const GameItem& mixItem = *iter;

            if (mixItem.GetQuantity() < material.quantity)
            {
                return false;
            }

            materials.push_back(ItemMixMaterial{
                .itemId = mixItem.GetId(),
                .quantity = material.quantity,
                });

            materialLevel += std::max(0, (mixItem.GetData().GetId() / itemIdGradeRuleConstant) - 1);
        }

        return true;
    }

    auto ItemMixTryMessageHandler::RollDiceAndGetResult(PlayerGroupSystem& system, const ItemMixDataProvider& dataProvider, const ItemMixGroupMemberData& data, int32_t gradeLevel) const -> const ItemData*
    {
        const std::array<int32_t, item_mix_grade_weight_size>* weightDataPtr = dataProvider.FindWeight(data.GetGradeType(), gradeLevel);
        if (!weightDataPtr)
        {
            SUNLIGHT_LOG_ERROR(system.GetServiceLocator(),
                fmt::format("fail to find item mix weight data. grade_type: {}, grade_level: {}",
                    data.GetGradeType(), gradeLevel));

            return nullptr;
        }

        std::array<int32_t, item_mix_grade_weight_size> weightData = *weightDataPtr;

        if (const std::optional<int32_t> difficulty = dataProvider.GetDifficulty(data.GetDifficulty(), gradeLevel);
            difficulty.has_value())
        {
            int32_t& failWeight = weightData[static_cast<int32_t>(ItemMixGradeWeightType::Fail)];

            failWeight = static_cast<int32_t>(std::round((static_cast<double>(failWeight) * static_cast<double>(*difficulty) / 100.0)));
        }

        int32_t prev = 0;
        boost::container::small_vector<std::pair<ItemMixGradeWeightType, int32_t>, item_mix_grade_weight_size> weights;

        for (int32_t i = 0; i < item_mix_grade_weight_size; ++i)
        {
            const int32_t value = weightData[i];
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

        const std::array<const ItemData*, item_mix_grade_count>& dataList = data.GetResultItemDataList();

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
        default:;
        }

        return nullptr;
    }

    auto ItemMixTryMessageHandler::CalculateItemMixExp(const ItemMixDataProvider& dataProvider, const ItemMixGroupMemberData& memberData, int32_t skillLevel, int32_t materialLevel) -> int32_t
    {
        const int32_t levelModifer = std::max(1, memberData.GetGroupLevel() * 2 / 3);
        const int32_t materialExp = materialLevel * 50;

        const int32_t baseExp = (400 + materialExp) * levelModifer;
        const int32_t modifier = dataProvider.GetExpModifier(memberData.GetDifficulty(), skillLevel).value_or(100);

        const int32_t result = static_cast<int32_t>(std::round(static_cast<double>(baseExp) * static_cast<double>(modifier) / 100.0));

        return result;
    }
}
