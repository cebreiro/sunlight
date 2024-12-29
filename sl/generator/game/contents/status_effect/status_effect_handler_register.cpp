#include "status_effect_handler_register.h"

#include "sl/generator/game/contents/status_effect/status_effect_handler.hpp"
#include "sl/generator/game/system/entity_status_effect_system.h"

namespace sunlight
{
    void PlayerSkillEffectStatusEffectHandlerRegister::Configure(EntityStatusEffectSystem& system)
    {
        system.AddHandler(std::make_unique<StatusEffectStatIncreaseHandler>());
    }
}
