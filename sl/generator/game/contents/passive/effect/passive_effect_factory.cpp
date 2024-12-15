#include "passive_effect_factory.h"

#include "sl/generator/game/contents/passive/effect/passive_effect.hpp"
#include "sl/generator/game/contents/passive/effect/impl/passive_effect_status_effect.h"
#include "sl/generator/service/gamedata/skill/skill_effect_data.h"

namespace sunlight
{
    auto PassiveEffectFactory::CreateEffect(const SkillEffectData& data) -> std::unique_ptr<IPassiveEffect>
    {
        switch (data.category)
        {
        case SkillEffectCategory::Damage:
        {
            if (data.type == 0)
            {
                return std::make_unique<PassiveEffectNormalAttackEnforce>(data);
            }
        }
        break;
        case SkillEffectCategory::StatusEffect:
        {
            return std::make_unique<PassiveEffectStatusEffect>(data);
        }
        break;
        case SkillEffectCategory::Stat:
        {
            return std::make_unique<PassiveEffectStat>(data);
        }
        break;
        case SkillEffectCategory::JobEffect: // TODO: implement
        case SkillEffectCategory::WeaponCondition:
        case SkillEffectCategory::Summon:
        case SkillEffectCategory::None:
            break;
        }

        return std::unique_ptr<IPassiveEffect>();
    }
}
