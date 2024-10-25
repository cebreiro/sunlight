#pragma once
#include <boost/container/small_vector.hpp>

namespace sunlight
{
    struct PlayerSkillData;
    struct MonsterSkillData;

    class GameSystem;

    class GameEntity;
    class GamePlayer;
    class GameMonster;
}

namespace sunlight
{
    class SkillTargetSelector
    {
    public:
        // skill_basic.sox -> DAMAGE_MAXCOUNT <= 8
        using result_type = boost::container::small_vector<PtrNotNull<GameEntity>, 8>;

    public:
        explicit SkillTargetSelector(GameSystem& system);

        bool SelectTarget(result_type& result, const GamePlayer& caster, const PlayerSkillData& skillData, GameEntity* optMainTarget) const;

    private:
        GameSystem& _system;
    };
}
