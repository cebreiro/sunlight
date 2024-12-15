#include "skill_target_selector.h"

#include "shared/collision/intersect.h"
#include "shared/collision/shape/circle.h"
#include "shared/collision/shape/obb.h"
#include "sl/generator/game/game_constant.h"
#include "sl/generator/game/component/player_party_component.h"
#include "sl/generator/game/component/scene_object_component.h"
#include "sl/generator/game/contents/sector/game_spatial_sector.h"
#include "sl/generator/game/entity/game_monster.h"
#include "sl/generator/game/entity/game_player.h"
#include "sl/generator/game/system/entity_skill_effect_system.h"
#include "sl/generator/game/system/entity_view_range_system.h"
#include "sl/generator/game/system/player_index_system.h"
#include "sl/generator/game/system/scene_object_system.h"
#include "sl/generator/service/gamedata/skill/monster_skill_data.h"
#include "sl/generator/service/gamedata/skill/player_skill_data.h"

namespace sunlight
{
    SkillTargetSelector::SkillTargetSelector(EntitySkillEffectSystem& system)
        : _system(system)
    {
    }

    bool SkillTargetSelector::SelectTarget(result_type& result, const GamePlayer& caster, const PlayerSkillData& skillData, const GameEntity* optMainTarget) const
    {
        switch (skillData.applyDamageType)
        {
        case SkillTargetingAreaType::OneUnit:
        {
            if (optMainTarget)
            {
                result.emplace_back(optMainTarget->GetId());

                return true;
            }
        }
        break;
        case SkillTargetingAreaType::Sphere:
        case SkillTargetingAreaType::OBB:
        {
            if (!optMainTarget)
            {
                return false;
            }

            const auto shape = [&]() -> std::variant<collision::Circle, collision::OBB>
                {
                    if (skillData.applyDamageType == SkillTargetingAreaType::Sphere)
                    {
                        const Eigen::Vector2f& center = optMainTarget->GetComponent<SceneObjectComponent>().GetPosition();

                        return collision::Circle(center, static_cast<float>(skillData.damageLength));
                    }
                    else if (skillData.applyDamageType == SkillTargetingAreaType::OBB)
                    {
                        const SceneObjectComponent& sceneObjectComponent = caster.GetSceneObjectComponent();
                        const Eigen::Vector2f& position = sceneObjectComponent.GetPosition();

                        const float yaw = [&]() -> float
                        {
                            if (optMainTarget)
                            {
                                const Eigen::Vector2f vector = optMainTarget->GetComponent<SceneObjectComponent>().GetPosition() - position;

                                return std::atan2(vector.y(), vector.x());
                            }

                            return sceneObjectComponent.GetYaw() * static_cast<float>(std::numbers::pi) / 180.f;;
                        }();

                        const Eigen::AlignedBox3f box(
                            Eigen::Vector3f(0, -static_cast<float>(skillData.damageLength2) / 2.f, 0.f),
                            Eigen::Vector3f(static_cast<float>(skillData.damageLength) + GameConstant::OBB_COLLISION_NETWORK_DELAY_MARGIN,
                                static_cast<float>(skillData.damageLength2) / 2.f, 0.f)
                        );

                        const Eigen::AngleAxisf axis(yaw, Eigen::Vector3f::UnitZ());
                        const auto rotation = axis.toRotationMatrix();
                        const Eigen::Vector3f localCenter = rotation * box.center();

                        return collision::OBB3f(Eigen::Vector3f(position.x(), position.y(), 0.f) + localCenter, box.sizes() * 0.5f, rotation).Project();
                    }

                    assert(false);

                    return collision::Circle{};
                }();

            auto hasIntersection = [&shape](const collision::Circle& circle) -> bool
                {
                return std::visit([&circle]<typename T>(const T& item)
                    {
                        return collision::Intersect(circle, item);

                    }, shape);
                };

            switch (skillData.applyTargetType)
            {
            case SkillTargetSelectType::Monster:
            case SkillTargetSelectType::Player:
            {
                const auto targetType = skillData.applyTargetType == SkillTargetSelectType::Player ? GameEntityType::Player : GameEntityType::Enemy;

                for (GameEntity& target : _system.Get<EntityViewRangeSystem>().GetSector(caster).GetEntities(targetType))
                {
                    const SceneObjectComponent& targetSceneObjectComponent = target.GetComponent<SceneObjectComponent>();
                    const auto& targetPosition = targetSceneObjectComponent.GetPosition();
                    const float targetBodySize = static_cast<float>(targetSceneObjectComponent.GetBodySize());

                    if (const collision::Circle targetShape(targetPosition, targetBodySize);
                        hasIntersection(targetShape))
                    {
                        result.push_back(target.GetId());

                        if (std::ssize(result) >= skillData.damageMaxCount)
                        {
                            break;
                        }
                    }
                }

                return true;
            }
            case SkillTargetSelectType::Party:
            {
                PlayerIndexSystem& playerIndexSystem = _system.Get<PlayerIndexSystem>();

                if (const PlayerPartyComponent& partyComponent = caster.GetPartyComponent();
                    partyComponent.HasParty())
                {
                    for (int64_t partyMemberId : partyComponent.GetMemberIds())
                    {
                        GamePlayer* partyMember = playerIndexSystem.FindByCId(partyMemberId);
                        if (!partyMember)
                        {
                            continue;
                        }

                        if (partyMemberId == caster.GetCId())
                        {
                            result.push_back(partyMember->GetId());

                            continue;
                        }

                        const SceneObjectComponent& targetSceneObjectComponent = partyMember->GetComponent<SceneObjectComponent>();

                        const auto& targetPosition = targetSceneObjectComponent.GetPosition();
                        const float targetBodySize = static_cast<float>(targetSceneObjectComponent.GetBodySize());

                        if (const collision::Circle targetShape(targetPosition, targetBodySize);
                            hasIntersection(targetShape))
                        {
                            result.push_back(partyMember->GetId());
                        }
                    }
                }
                else
                {
                    result.push_back(caster.GetId());
                }

                return true;
            }
            case SkillTargetSelectType::MaybeGroundPoint:
            case SkillTargetSelectType::PlayerSelf:
                return false;
            }
        }
        break;
        default: ;
        }

        return false;
    }

    bool SkillTargetSelector::SelectTarget(result_type& result, const GameMonster& caster, const MonsterSkillData& skillData, const GameEntity& mainTarget) const
    {
        if (skillData.applyTargetType == SkillTargetSelectType::MonsterSelf)
        {
            result.emplace_back(caster.GetId());

            return true;
        }

        switch (skillData.applyDamageType)
        {
        case SkillTargetingAreaType::OneUnit:
        {
            if (skillData.applyTargetType == SkillTargetSelectType::Monster)
            {
                result.emplace_back(caster.GetId());

                return true;
            }
            else if (skillData.applyTargetType == SkillTargetSelectType::Player)
            {
                result.emplace_back(mainTarget.GetId());

                return true;
            }
        }
        break;
        case SkillTargetingAreaType::Sphere:
        case SkillTargetingAreaType::OBB:
        {
            const auto shape = [&]() -> std::variant<collision::Circle, collision::OBB>
                {
                    if (skillData.applyDamageType == SkillTargetingAreaType::Sphere)
                    {
                        const Eigen::Vector2f& center = mainTarget.GetComponent<SceneObjectComponent>().GetPosition();

                        return collision::Circle(center, static_cast<float>(skillData.damageLength));
                    }
                    else if (skillData.applyDamageType == SkillTargetingAreaType::OBB)
                    {
                        const SceneObjectComponent& sceneObjectComponent = caster.GetSceneObjectComponent();
                        const Eigen::Vector2f& position = sceneObjectComponent.GetPosition();

                        const float yaw = [&]() -> float
                            {
                                const Eigen::Vector2f vector = mainTarget.GetComponent<SceneObjectComponent>().GetPosition() - position;

                                return std::atan2(vector.y(), vector.x());
                            }();

                        const Eigen::AlignedBox3f box(
                            Eigen::Vector3f(0, -static_cast<float>(skillData.damageLength2) / 2.f, 0.f),
                            Eigen::Vector3f(static_cast<float>(skillData.damageLength) + GameConstant::OBB_COLLISION_NETWORK_DELAY_MARGIN,
                                static_cast<float>(skillData.damageLength2) / 2.f, 0.f)
                        );

                        const Eigen::AngleAxisf axis(yaw, Eigen::Vector3f::UnitZ());
                        const auto rotation = axis.toRotationMatrix();
                        const Eigen::Vector3f localCenter = rotation * box.center();

                        return collision::OBB3f(Eigen::Vector3f(position.x(), position.y(), 0.f) + localCenter, box.sizes() * 0.5f, rotation).Project();
                    }

                    assert(false);

                    return collision::Circle{};
                }();

            auto hasIntersection = [&shape](const collision::Circle& circle) -> bool
                {
                    return std::visit([&circle]<typename T>(const T & item)
                    {
                        return collision::Intersect(circle, item);

                    }, shape);
                };

            switch (skillData.applyTargetType)
            {
            case SkillTargetSelectType::Monster:
            case SkillTargetSelectType::Player:
            {
                const auto targetType = skillData.applyTargetType == SkillTargetSelectType::Player ? GameEntityType::Player : GameEntityType::Enemy;

                for (GameEntity& target : _system.Get<EntityViewRangeSystem>().GetSector(caster).GetEntities(targetType))
                {
                    const SceneObjectComponent& targetSceneObjectComponent = target.GetComponent<SceneObjectComponent>();
                    const auto& targetPosition = targetSceneObjectComponent.GetPosition();
                    const float targetBodySize = static_cast<float>(targetSceneObjectComponent.GetBodySize());

                    if (const collision::Circle targetShape(targetPosition, targetBodySize);
                        hasIntersection(targetShape))
                    {
                        result.push_back(target.GetId());

                        if (std::ssize(result) >= skillData.damageMaxCount)
                        {
                            break;
                        }
                    }
                }

                return true;
            }
            }
        }
        break;
        }

        return false;
    }
}
