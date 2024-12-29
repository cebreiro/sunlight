#include "server_command_status_effect.h"

#include "sl/generator/game/entity/game_player.h"
#include "sl/generator/game/system/entity_status_effect_system.h"
#include "sl/generator/game/system/server_command_system.h"
#include "sl/generator/service/gamedata/gamedata_provide_service.h"
#include "sl/generator/service/gamedata/skill/skill_data_provider.h"
#include "sl/generator/service/gamedata/skill/player_skill_data.h"

namespace sunlight
{
    ServerCommandStatusEffectPlayerAdd::ServerCommandStatusEffectPlayerAdd(ServerCommandSystem& system)
        : _system(system)
    {
    }

    auto ServerCommandStatusEffectPlayerAdd::GetName() const -> std::string_view
    {
        return "status_effect_player_add";
    }

    auto ServerCommandStatusEffectPlayerAdd::GetRequiredGmLevel() const -> int8_t
    {
        return 0;
    }

    bool ServerCommandStatusEffectPlayerAdd::Execute(GamePlayer& player, int32_t skillId, int32_t skillLevel) const
    {
        const SkillDataProvider& skillDataProvider = _system.GetServiceLocator().Get<GameDataProvideService>().GetSkillDataProvider();
        const PlayerSkillData* skillData = skillDataProvider.FindPlayerSkill(skillId);

        if (!skillData)
        {
            player.Notice(fmt::format("fail to find player skill data. skill_id: {}", skillId));

            return false;
        }

        std::array<game_entity_id_type, 1> targets{ player.GetId() };

        for (const SkillEffectData& effect : skillData->effects)
        {
            if (effect.category == SkillEffectCategory::StatusEffect)
            {
                player.Notice(fmt::format("status effect applied. skill: {}, status_effect: {}",
                    skillId, ToString(static_cast<StatusEffectType>(effect.type))));

                _system.Get<EntityStatusEffectSystem>().AddStatusEffectBySkill(skillId, skillLevel, targets, effect);
            }
        }

        return true;
    }
}
