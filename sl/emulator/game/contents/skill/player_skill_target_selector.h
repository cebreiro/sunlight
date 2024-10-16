#pragma once
#include <boost/container/small_vector.hpp>

namespace sunlight
{
    struct PlayerSkillData;

    class GameEntity;
    class GamePlayer;
    class PlayerSkillEffectSystem;
}

namespace sunlight
{
    class PlayerSkillTargetSelector
    {
    public:
        // skill_basic.sox -> DAMAGE_MAXCOUNT <= 8
        using result_type = boost::container::small_vector<PtrNotNull<GameEntity>, 8>;

    public:
        explicit PlayerSkillTargetSelector(PlayerSkillEffectSystem& system);

        bool SelectTarget(result_type& result, const GamePlayer& caster, const PlayerSkillData& skillData, GameEntity* optMainTarget) const;

    private:
        PlayerSkillEffectSystem& _system;
    };
}
