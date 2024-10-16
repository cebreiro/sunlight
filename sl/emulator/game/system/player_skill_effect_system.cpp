#include "player_skill_effect_system.h"

#include "sl/emulator/game/component/player_skill_component.h"
#include "sl/emulator/game/contents/skill/player_skill_target_selector.h"
#include "sl/emulator/game/entity/game_player.h"
#include "sl/emulator/game/system/entity_status_effect_system.h"
#include "sl/emulator/game/system/entity_view_range_system.h"
#include "sl/emulator/game/system/player_index_system.h"
#include "sl/emulator/game/system/scene_object_system.h"
#include "sl/emulator/game/zone/stage.h"
#include "sl/emulator/service/gamedata/skill/player_skill_data.h"

namespace sunlight
{
    PlayerSkillEffectSystem::PlayerSkillEffectSystem(const ServiceLocator& serviceLocator)
        : _serviceLocator(serviceLocator)
        , _skillTargetSelector(std::make_unique<PlayerSkillTargetSelector>(*this))
    {
    }

    PlayerSkillEffectSystem::~PlayerSkillEffectSystem()
    {
    }

    void PlayerSkillEffectSystem::InitializeSubSystem(Stage& stage)
    {
        Add(stage.Get<SceneObjectSystem>());
        Add(stage.Get<EntityViewRangeSystem>());
        Add(stage.Get<PlayerIndexSystem>());
        Add(stage.Get<EntityStatusEffectSystem>());
    }

    bool PlayerSkillEffectSystem::Subscribe(Stage& stage)
    {
        (void)stage;

        return true;
    }

    auto PlayerSkillEffectSystem::GetName() const -> std::string_view
    {
        return "player_skill_effect_system";
    }

    auto PlayerSkillEffectSystem::GetClassId() const -> game_system_id_type
    {
        return GameSystem::GetClassId<PlayerSkillEffectSystem>();
    }

    void PlayerSkillEffectSystem::OnSkillUse(GamePlayer& player, int32_t skillId, game_entity_id_type targetId, GameEntityType targetType, int32_t chargeTime)
    {
        (void)chargeTime;

        PlayerSkillComponent& skillComponent = player.GetSkillComponent();

        PlayerSkill* skill = skillComponent.FindSkill(skillId);
        if (!skill)
        {
            return;
        }

        GameEntity* mainTarget = targetType != GameEntityType::None ? Get<SceneObjectSystem>().FindEntity(targetType, targetId).get() : nullptr;

        PlayerSkillTargetSelector::result_type skillTargets;
        if (!_skillTargetSelector->SelectTarget(skillTargets, player, skill->GetData(), mainTarget))
        {
            player.Notice(fmt::format("fail to select skill target. skill: {}", skillId));

            return;
        }

        for (GameEntity* target : skillTargets)
        {
            player.Notice(fmt::format("skill: {}, target: [{}, {}]", skillId, target->GetId(), ToString(target->GetType())));
        }

        const PlayerSkillData& data = skill->GetData();

        if (!data.statusEffects.empty())
        {
            Get<EntityStatusEffectSystem>().AddStatusEffectBySkill(skillId, skill->GetLevel(), skillTargets, data.statusEffects);
        }

        if (data.damage)
        {
            // TODO: do process damage
        }
    }
}
