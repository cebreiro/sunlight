#pragma once
#include <boost/container/small_vector.hpp>
#include "sl/generator/game/entity/game_entity_id_type.h"

namespace sunlight
{
    struct PlayerSkillData;
    struct MonsterSkillData;

    class EntitySkillEffectSystem;

    class GameEntity;
    class GamePlayer;
    class GameMonster;
}

namespace sunlight
{
    class SkillTargetSelector
    {
    public:
        using result_type = boost::container::small_vector<game_entity_id_type, 8>;

    public:
        explicit SkillTargetSelector(EntitySkillEffectSystem& system);

        bool SelectTarget(result_type& result, const GamePlayer& caster, const PlayerSkillData& skillData, const GameEntity* optMainTarget) const;
        bool SelectTarget(result_type& result, const GameMonster& caster, const MonsterSkillData& skillData, const GameEntity& mainTarget) const;

    private:
        EntitySkillEffectSystem& _system;
    };
}
