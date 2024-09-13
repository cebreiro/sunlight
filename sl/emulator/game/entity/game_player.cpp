#include "game_player.h"

#include "sl/emulator/game/component/player_appearance_component.h"
#include "sl/emulator/game/component/player_item_component.h"
#include "sl/emulator/game/component/player_job_component.h"
#include "sl/emulator/game/component/player_skill_component.h"
#include "sl/emulator/game/component/player_stat_component.h"
#include "sl/emulator/game/component/scene_object_component.h"
#include "sl/emulator/game/contants/item/equipment_position.h"
#include "sl/emulator/game/entity/game_entity_id_pool.h"
#include "sl/emulator/server/client/game_client.h"
#include "sl/emulator/service/gamedata/gamedata_provide_service.h"
#include "sl/emulator/service/gamedata/item/item_data_provider.h"
#include "sl/emulator/service/gamedata/skill/skill_data_provider.h"

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

                if (!result->AddSkill(playerSkill))
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

        result->SetPosition(Eigen::Vector3f(dto.x, dto.y, 0.f));

        return result;
    }

    GamePlayer::GamePlayer(SharedPtrNotNull<GameClient> client, const db::dto::Character& dto, const GameDataProvideService& dataProvider,
        GameEntityIdPool& idPool)
        : GameEntity(idPool.Pop(TYPE), TYPE)
        , _cid(dto.id)
        , _aid(dto.aid)
        , _name(dto.name)
        , _zoneId(dto.zone)
        , _client(std::move(client))
    {
        (void)AddComponent(CreatePlayerAppearanceComponent(dataProvider.GetItemDataProvider(), dto));
        // item
        (void)AddComponent(std::make_unique<PlayerJobComponent>(dataProvider, dto.jobs));
        (void)AddComponent(CreateSkillComponent(dataProvider.GetSkillDataProvider(), dto));
        (void)AddComponent(std::make_unique<PlayerStatComponent>(dto));
        (void)AddComponent(CreateSceneObjectComponent(dto));
    }

    bool GamePlayer::IsArmed() const
    {
        return _armed;
    }

    bool GamePlayer::IsRunning() const
    {
        return _running;
    }

    void GamePlayer::Send(Buffer buffer)
    {
        assert(_client);

        _client->Send(ServerType::Zone, std::move(buffer));
    }

    auto GamePlayer::GetCid() const -> int64_t
    {
        return _cid;
    }

    auto GamePlayer::GetAid() const -> int64_t
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

    auto GamePlayer::GetClientId() const -> game_client_id_type
    {
        assert(_client);

        return _client->GetId();
    }

    auto GamePlayer::GetStage() -> Stage&
    {
        assert(_stage);

        return *_stage;
    }

    auto GamePlayer::GetStage() const -> const Stage&
    {
        assert(_stage);

        return *_stage;
    }

    void GamePlayer::SetStage(Stage* stage)
    {
        _stage = stage;
    }

    void GamePlayer::SetArmed(bool value)
    {
        _armed = value;
    }

    void GamePlayer::SetRunning(bool value)
    {
        _running = value;
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
}
