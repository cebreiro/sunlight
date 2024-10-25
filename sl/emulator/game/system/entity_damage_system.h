#pragma once
#include "sl/emulator/game/entity/game_entity_id_type.h"
#include "sl/emulator/game/system/game_system.h"
#include "sl/emulator/service/gamedata/item/weapon_class_type.h"

namespace sunlight
{
    struct AbilityValue;
    struct SkillEffectData;
    struct DamageResult;
    struct MonsterSkillData;

    class ItemData;
    class PlayerSkill;

    class GameEntity;
    class GamePlayer;
    class GameMonster;

    class IPlayerAttackDamageCalculator;
}

namespace sunlight
{
    class EntityDamageSystem final : public GameSystem
    {
    public:
        EntityDamageSystem(const ServiceLocator& serviceLocator, int32_t stageId);
        ~EntityDamageSystem();

        void InitializeSubSystem(Stage& stage) override;
        bool Subscribe(Stage& stage) override;
        auto GetName() const -> std::string_view override;
        auto GetClassId() const -> game_system_id_type override;

    public:
        void KillMonster(GamePlayer& player, game_entity_id_type mobId);

        void ProcessPlayerSkillEffect(GamePlayer& player, GameMonster& target, const PlayerSkill& skill, const SkillEffectData& effect,
            int32_t attackId, int32_t chargeCount, WeaponClassType weaponClass, const AbilityValue* abilityValue);

        void ProcessMonsterNormalAttack(GameMonster& monster, GameEntity& target);
        void ProcessMonsterSkillEffect(GameMonster& monster, GameEntity& target, const MonsterSkillData& skillData,
            const SkillEffectData& effect, const AbilityValue* abilityValue);

    private:
        void OnDelayDamage(int64_t playerId, game_entity_id_type targetMonsterId, int32_t damage);

        void ProcessMonsterDead(const GamePlayer& player, GameMonster& monster, const DamageResult* damageResult);
        void DropMonsterItem(const GameMonster& monster, const GamePlayer* player);

    private:
        const ServiceLocator& _serviceLocator;
        int32_t _stageId = 0;

        UniquePtrNotNull<IPlayerAttackDamageCalculator> _damageCalculator;

        std::vector<std::pair<PtrNotNull<const ItemData>, int32_t>> _dropItemQueryResult;
        std::mt19937 _mt;
    };
}
