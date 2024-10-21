#pragma once
#include "sl/emulator/game/contents/damage/damage_result_type.h"
#include "sl/emulator/game/entity/game_entity_id_type.h"
#include "sl/emulator/game/entity/game_entity_type.h"
#include "sl/emulator/service/gamedata/item/weapon_class_type.h"

namespace sunlight
{
    struct DamageResult
    {
        game_entity_id_type attackerId = {};
        GameEntityType attackerType = {};

        DamageType damageType = DamageType::None;
        int32_t id = 0;

        // motionId 3 -> display skill effect to attacked
        int32_t motionId = 0; // equal to GameEntityState.motionId

        int32_t skillId = 0;
        WeaponClassType weaponClass = WeaponClassType::None;

        // client 0x493367 damage 0 -> miss (miss != dodge)
        int32_t damage = 0;

        // client displays damage, divided by the attackCount, at each attackInterval
        int32_t damageCount = 1;
        int32_t damageInterval = 0;

        // client 0x491802
        // 0 -> default: 0x10000969
        int32_t blowGroup = 0;
        DamageBlowType blowType = DamageBlowType::BlowSmall;

        DamageResultType attackedResultType = DamageResultType::Damage_A;
    };
}
