#include "game_player.h"

#include "sl/generator/game/component/entity_immune_component.h"
#include "sl/generator/game/component/entity_movement_component.h"
#include "sl/generator/game/component/entity_passive_effect_component.h"
#include "sl/generator/game/component/entity_state_component.h"
#include "sl/generator/game/component/entity_status_effect_component.h"
#include "sl/generator/game/component/player_appearance_component.h"
#include "sl/generator/game/component/player_debug_component.h"
#include "sl/generator/game/component/player_group_component.h"
#include "sl/generator/game/component/player_item_component.h"
#include "sl/generator/game/component/player_job_component.h"
#include "sl/generator/game/component/player_npc_script_component.h"
#include "sl/generator/game/component/player_npc_shop_component.h"
#include "sl/generator/game/component/player_party_component.h"
#include "sl/generator/game/component/player_profile_component.h"
#include "sl/generator/game/component/player_quest_component.h"
#include "sl/generator/game/component/player_skill_component.h"
#include "sl/generator/game/component/player_stat_component.h"
#include "sl/generator/game/component/scene_object_component.h"
#include "sl/generator/game/contents/event_script/event_script.h"
#include "sl/generator/game/contents/item/equipment_position.h"
#include "sl/generator/game/contents/npc/npc_talk_box.h"
#include "sl/generator/game/contents/passive/passive.h"
#include "sl/generator/game/message/creator/chat_message_creator.h"
#include "sl/generator/game/message/creator/game_player_message_creator.h"
#include "sl/generator/game/message/creator/npc_message_creator.h"
#include "sl/generator/server/client/game_client.h"
#include "sl/generator/service/gamedata/gamedata_provide_service.h"
#include "sl/generator/service/gamedata/item/item_data_provider.h"
#include "sl/generator/service/gamedata/skill/skill_data_provider.h"

namespace sunlight
{
    auto CreatePlayerAppearanceComponent(const ItemDataProvider& itemDataProvider, const db::dto::Character& dto) -> UniquePtrNotNull<PlayerAppearanceComponent>
    {
        const auto findModel = [&itemDataProvider](int32_t itemId) -> std::optional<std::pair<int32_t, int32_t>>
            {
                if (itemId == 0)
                {
                    return std::nullopt;
                }

                const ItemData* data = itemDataProvider.Find(itemId);
                if (!data)
                {
                    return std::nullopt;
                }

                return std::make_pair(data->GetModelId(), data->GetModelColor());
            };

        auto result = std::make_unique<PlayerAppearanceComponent>();

        result->SetHair(dto.hair);
        result->SetHairColor(dto.hairColor);
        result->SetFace(dto.face);
        result->SetSkinColor(dto.skinColor);

        for (const db::dto::Character::Item& item : dto.items | std::views::filter([](const db::dto::Character::Item& item)
            {
                return item.posType == db::ItemPosType::Equipment;
            }))
        {
            switch (static_cast<EquipmentPosition>(item.page))
            {
            case EquipmentPosition::Hat:
            {
                if (const auto opt = findModel(item.dataId); opt.has_value())
                {
                    result->SetHatModelId(opt->first);
                    result->SetHatModelColor(opt->second);
                }
            }
            break;
            case EquipmentPosition::Jacket:
            {
                if (const auto opt = findModel(item.dataId); opt.has_value())
                {
                    result->SetJacketModelId(opt->first);
                    result->SetJacketModelColor(opt->second);
                }
            }
            break;
            case EquipmentPosition::Gloves:
            {
                if (const auto opt = findModel(item.dataId); opt.has_value())
                {
                    result->SetGlovesModelId(opt->first);
                    result->SetGlovesModelColor(opt->second);
                }
            }
            break;
            case EquipmentPosition::Pants:
            {
                if (const auto opt = findModel(item.dataId); opt.has_value())
                {
                    result->SetPantsModelId(opt->first);
                    result->SetPantsModelColor(opt->second);
                }
            }
            break;
            case EquipmentPosition::Shoes:
            {
                if (const auto opt = findModel(item.dataId); opt.has_value())
                {
                    result->SetShoesModelId(opt->first);
                    result->SetShoesModelColor(opt->second);
                }
            }
            break;
            default:;
            }
        }

        return result;
    }

    auto CreateSkillComponent(const SkillDataProvider& skillDataProvider, const db::dto::Character& dto) -> UniquePtrNotNull<PlayerSkillComponent>
    {
        auto result = std::make_unique<PlayerSkillComponent>();

        for (const db::dto::Character::Skill& dtoSkill : dto.skills)
        {
            if (const PlayerSkillData* data = skillDataProvider.FindPlayerSkill(dtoSkill.id); data)
            {
                PlayerSkill playerSkill(static_cast<JobId>(dtoSkill.job), data);
                playerSkill.SetBaseLevel(dtoSkill.level);
                playerSkill.SetCooldown(dtoSkill.cooldown);
                playerSkill.SetPage(dtoSkill.page);
                playerSkill.SetX(dtoSkill.x);
                playerSkill.SetY(dtoSkill.y);
                playerSkill.SetEXP(dtoSkill.exp);

                if (!result->AddSkill(std::move(playerSkill)))
                {
                    throw std::runtime_error(fmt::format("fail to add player_skill. skill_id: {}", dtoSkill.id));
                }
            }
            else
            {
                throw std::runtime_error(fmt::format("fail to find skill data. skill_id: {}", dtoSkill.id));
            }
        }

        return result;
    }

    auto CreateSceneObjectComponent(const db::dto::Character& dto) -> UniquePtrNotNull<SceneObjectComponent>
    {
        auto result = std::make_unique<SceneObjectComponent>();

        result->SetPosition(Eigen::Vector2f(dto.x, dto.y));
        result->SetYaw(dto.yaw);

        return result;
    }

    auto CreateQuestComponent(const db::dto::Character& dto) -> UniquePtrNotNull<PlayerQuestComponent>
    {
        auto result = std::make_unique<PlayerQuestComponent>();

        for (const db::dto::Character::Quest& dtoQuest : dto.quests)
        {
            std::optional<Quest> quest = Quest::CreateFrom(dtoQuest.id, dtoQuest.state, dtoQuest.flags, dtoQuest.data);
            if (!quest.has_value())
            {
                throw std::runtime_error(fmt::format("invalid quest data. player: {}, quest_id: {}",
                    dto.id, dtoQuest.id));
            }

            result->AddQuest(std::move(*quest));
        }

        return result;
    }

    auto CreateProfileComponent(const db::dto::Character& dto) -> UniquePtrNotNull<PlayerProfileComponent>
    {
        auto result = std::make_unique<PlayerProfileComponent>();

        result->Configure(PlayerProfileSetting::RefusePartyInvite, dto.profileSetting.refusePartyInvite != 0);
        result->Configure(PlayerProfileSetting::RefuseChannelInvite, dto.profileSetting.refuseChannelInvite != 0);
        result->Configure(PlayerProfileSetting::RefuseGuildInvite, dto.profileSetting.refuseGuildInvite != 0);
        result->Configure(PlayerProfileSetting::Private, dto.profileSetting.privateProfile != 0);

        return result;
    }

    GamePlayer::GamePlayer(SharedPtrNotNull<GameClient> client, const db::dto::Character& dto,
        const GameDataProvideService& dataProvider, GameEntityIdPublisher& idPublisher)
        : GameEntity(idPublisher, TYPE)
        , _cid(dto.id)
        , _aid(dto.aid)
        , _name(dto.name)
        , _zoneId(dto.zone)
        , _client(std::move(client))
    {
        _armed = dto.arms;
        _running = dto.running;
        _dead = dto.dead;

        (void)AddComponent(CreatePlayerAppearanceComponent(dataProvider.GetItemDataProvider(), dto));
        (void)AddComponent(std::make_unique<PlayerItemComponent>(idPublisher, dataProvider.GetItemDataProvider(), dto));
        (void)AddComponent(std::make_unique<PlayerJobComponent>(dataProvider, dto.jobs));
        (void)AddComponent(CreateSkillComponent(dataProvider.GetSkillDataProvider(), dto));
        (void)AddComponent(std::make_unique<PlayerStatComponent>(dto));
        (void)AddComponent(CreateSceneObjectComponent(dto));
        (void)AddComponent(std::make_unique<EntityMovementComponent>());
        (void)AddComponent(std::make_unique<EntityStateComponent>());
        (void)AddComponent(std::make_unique<PlayerNPCScriptComponent>());
        (void)AddComponent(CreateQuestComponent(dto));
        (void)AddComponent(std::make_unique<PlayerNPCShopComponent>());
        (void)AddComponent(CreateProfileComponent(dto));
        (void)AddComponent(std::make_unique<PlayerGroupComponent>());
        (void)AddComponent(std::make_unique<PlayerPartyComponent>());
        (void)AddComponent(std::make_unique<EntityStatusEffectComponent>());
        (void)AddComponent(std::make_unique<PlayerDebugComponent>());
        (void)AddComponent(std::make_unique<EntityPassiveEffectComponent>());
        (void)AddComponent(std::make_unique<EntityImmuneComponent>());

        GetSceneObjectComponent().SetBodySize(GameConstant::PLAYER_BODY_SIZE);
    }

    bool GamePlayer::HasDeferred() const
    {
        return !_deferred.empty();
    }

    void GamePlayer::Defer(Buffer buffer)
    {
        _deferred.emplace_back(std::move(buffer));
    }

    void GamePlayer::FlushDeferred()
    {
        assert(_client);

        _client->Send(ServerType::Zone, std::move(_deferred));

        constexpr int64_t reservedMagicNumberSize = 8;
        _deferred.reserve(reservedMagicNumberSize);
    }

    void GamePlayer::Send(Buffer buffer)
    {
        assert(_client);

        _client->Send(ServerType::Zone, std::move(buffer));
    }

    void GamePlayer::Notice(const std::string& message)
    {
        Send(ChatMessageCreator::CreateServerMessage(message));
    }

    void GamePlayer::Talk(const GameNPC& npc, const NPCTalkBox& talkBox)
    {
        Defer(NPCMessageCreator::CreateTalkBoxClear(npc));

        for (const npc_talk_box_item_type& element : talkBox.GetTalkBoxItems())
        {
            std::visit([&]<typename T>(const T & item)
            {
                if constexpr (std::is_same_v<T, NPCTalkBoxString>)
                {
                    Defer(NPCMessageCreator::CreateTalkBoxAddString(npc, item.tableIndex));
                }
                else if constexpr (std::is_same_v<T, NPCTalkBoxStringWithInt>)
                {
                    Defer(NPCMessageCreator::CreateTalkBoxAddRuntimeIntString(npc, item.tableIndex, item.value));
                }
                else if constexpr (std::is_same_v<T, NPCTalkBoxStringWithItem>)
                {
                    Defer(NPCMessageCreator::CreateTalkBoxAddItemName(npc, item.tableIndex, item.tableIndex));
                }
                else if constexpr (std::is_same_v<T, NPCTalkBoxMenu>)
                {
                    Defer(NPCMessageCreator::CreateTalkBoxAddMenu(npc, item.tableIndexDefault, item.tableIndexMouseOver, item.index));
                }
                else
                {
                    static_assert(sizeof(T), "not implemented");
                }

            }, element);
        }

        Defer(NPCMessageCreator::CreateTalkBoxCreate(npc, *this,talkBox.GetWidth(), talkBox.GetHeight()));
        FlushDeferred();
    }

    void GamePlayer::Show(const EventScript& eventScript)
    {
        Defer(GamePlayerMessageCreator::CreateEventScriptClear(*this));

        for (const event_script_item_type& element : eventScript.GetItems())
        {
            std::visit([&]<typename T>(const T & item)
            {
                if constexpr (std::is_same_v<T, EventScriptString>)
                {
                    Defer(GamePlayerMessageCreator::CreateEventScriptAddString(*this, item.tableIndex));
                }
                else if constexpr (std::is_same_v<T, EventScriptStringWithInt>)
                {
                    Defer(GamePlayerMessageCreator::CreateEventScriptAddStringWithInt(*this, item.tableIndex, item.value));
                }
                else if constexpr (std::is_same_v<T, EventScriptStringWithItem>)
                {
                    Defer(GamePlayerMessageCreator::CreateEventScriptAddStringWithItem(*this, item.tableIndex, item.itemId));
                }
                else
                {
                    static_assert(sizeof(T), "not implemented");
                }

            }, element);
        }

        Defer(GamePlayerMessageCreator::CreateEventScriptShow(*this));
        FlushDeferred();
    }

    bool GamePlayer::IsArmed() const
    {
        return _armed;
    }

    bool GamePlayer::IsRunning() const
    {
        return _running;
    }

    bool GamePlayer::IsDead() const
    {
        return _dead;
    }

    auto GamePlayer::GetCId() const -> int64_t
    {
        return _cid;
    }

    auto GamePlayer::GetAId() const -> int64_t
    {
        return _aid;
    }

    auto GamePlayer::GetName() const -> const std::string&
    {
        return _name;
    }

    auto GamePlayer::GetZoneId() const -> int32_t
    {
        return _zoneId;
    }

    auto GamePlayer::GetGmLevel() const -> int8_t
    {
        return _gmLevel;
    }

    auto GamePlayer::GetClient() const -> GameClient&
    {
        assert(_client);

        return *_client;
    }

    auto GamePlayer::GetClientId() const -> game_client_id_type
    {
        assert(_client);

        return _client->GetId();
    }

    void GamePlayer::SetArmed(bool value)
    {
        _armed = value;
    }

    void GamePlayer::SetRunning(bool value)
    {
        _running = value;
    }

    void GamePlayer::SetDead(bool value)
    {
        _dead = value;
    }

    auto GamePlayer::GetAppearanceComponent() -> PlayerAppearanceComponent&
    {
        return GetComponent<PlayerAppearanceComponent>();
    }

    auto GamePlayer::GetAppearanceComponent() const -> const PlayerAppearanceComponent&
    {
        return GetComponent<PlayerAppearanceComponent>();
    }

    auto GamePlayer::GetItemComponent() -> PlayerItemComponent&
    {
        return GetComponent<PlayerItemComponent>();
    }

    auto GamePlayer::GetItemComponent() const -> const PlayerItemComponent&
    {
        return GetComponent<PlayerItemComponent>();
    }

    auto GamePlayer::GetJobComponent() -> PlayerJobComponent&
    {
        return GetComponent<PlayerJobComponent>();
    }

    auto GamePlayer::GetJobComponent() const -> const PlayerJobComponent&
    {
        return GetComponent<PlayerJobComponent>();
    }

    auto GamePlayer::GetSkillComponent() -> PlayerSkillComponent&
    {
        return GetComponent<PlayerSkillComponent>();
    }

    auto GamePlayer::GetSkillComponent() const -> const PlayerSkillComponent&
    {
        return GetComponent<PlayerSkillComponent>();
    }

    auto GamePlayer::GetStatComponent() -> PlayerStatComponent&
    {
        return GetComponent<PlayerStatComponent>();
    }

    auto GamePlayer::GetStatComponent() const -> const PlayerStatComponent&
    {
        return GetComponent<PlayerStatComponent>();
    }

    auto GamePlayer::GetSceneObjectComponent() -> SceneObjectComponent&
    {
        return GetComponent<SceneObjectComponent>();
    }

    auto GamePlayer::GetSceneObjectComponent() const -> const SceneObjectComponent&
    {
        return GetComponent<SceneObjectComponent>();
    }

    auto GamePlayer::GetMovementComponent() -> EntityMovementComponent&
    {
        return GetComponent<EntityMovementComponent>();
    }

    auto GamePlayer::GetMovementComponent() const -> const EntityMovementComponent&
    {
        return GetComponent<EntityMovementComponent>();
    }

    auto GamePlayer::GetStateComponent() -> EntityStateComponent&
    {
        return GetComponent<EntityStateComponent>();
    }

    auto GamePlayer::GetStateComponent() const -> const EntityStateComponent&
    {
        return GetComponent<EntityStateComponent>();
    }

    auto GamePlayer::GetNPCScriptComponent() -> PlayerNPCScriptComponent&
    {
        return GetComponent<PlayerNPCScriptComponent>();
    }

    auto GamePlayer::GetNPCScriptComponent() const -> const PlayerNPCScriptComponent&
    {
        return GetComponent<PlayerNPCScriptComponent>();
    }

    auto GamePlayer::GetQuestComponent() -> PlayerQuestComponent&
    {
        return GetComponent<PlayerQuestComponent>();
    }

    auto GamePlayer::GetQuestComponent() const -> const PlayerQuestComponent&
    {
        return GetComponent<PlayerQuestComponent>();
    }

    auto GamePlayer::GetNPCShopComponent() -> PlayerNPCShopComponent&
    {
        return GetComponent<PlayerNPCShopComponent>();
    }

    auto GamePlayer::GetNPCShopComponent() const -> const PlayerNPCShopComponent&
    {
        return GetComponent<PlayerNPCShopComponent>();
    }

    auto GamePlayer::GetProfileComponent() -> PlayerProfileComponent&
    {
        return GetComponent<PlayerProfileComponent>();
    }

    auto GamePlayer::GetProfileComponent() const -> const PlayerProfileComponent&
    {
        return GetComponent<PlayerProfileComponent>();
    }

    auto GamePlayer::GetGroupComponent() -> PlayerGroupComponent&
    {
        return GetComponent<PlayerGroupComponent>();
    }

    auto GamePlayer::GetGroupComponent() const -> const PlayerGroupComponent&
    {
        return GetComponent<PlayerGroupComponent>();
    }

    auto GamePlayer::GetPartyComponent() -> PlayerPartyComponent&
    {
        return GetComponent<PlayerPartyComponent>();
    }

    auto GamePlayer::GetPartyComponent() const -> const PlayerPartyComponent&
    {
        return GetComponent<PlayerPartyComponent>();
    }

    auto GamePlayer::GetStatusEffectComponent() -> EntityStatusEffectComponent&
    {
        return GetComponent<EntityStatusEffectComponent>();
    }

    auto GamePlayer::GetStatusEffectComponent() const -> const EntityStatusEffectComponent&
    {
        return GetComponent<EntityStatusEffectComponent>();
    }

    auto GamePlayer::GetDebugComponent() -> PlayerDebugComponent&
    {
        return GetComponent<PlayerDebugComponent>();
    }

    auto GamePlayer::GetDebugComponent() const -> const PlayerDebugComponent&
    {
        return GetComponent<PlayerDebugComponent>();
    }

    auto GamePlayer::GetPassiveEffectComponent() -> EntityPassiveEffectComponent&
    {
        return GetComponent<EntityPassiveEffectComponent>();
    }

    auto GamePlayer::GetPassiveEffectComponent() const -> const EntityPassiveEffectComponent&
    {
        return GetComponent<EntityPassiveEffectComponent>();
    }

    auto GamePlayer::GetImmuneComponent() -> EntityImmuneComponent&
    {
        return GetComponent<EntityImmuneComponent>();
    }

    auto GamePlayer::GetImmuneComponent() const -> const EntityImmuneComponent&
    {
        return GetComponent<EntityImmuneComponent>();
    }
}
