#include "monster_aggro_component.h"

#include "sl/generator/game/data/sox/monster_action.h"
#include "sl/generator/game/time/game_time_service.h"

namespace sunlight
{
    MonsterAggroComponent::MonsterAggroComponent(const sox::MonsterAction& data)
        : _data(data)
    {
    }

    bool MonsterAggroComponent::Empty() const
    {
        return _descSortedArray.empty();
    }

    bool MonsterAggroComponent::Remove(game_entity_id_type targetId)
    {
        auto iter = std::ranges::find_if(_descSortedArray, [targetId](const Aggro& aggro)
            {
                return targetId == aggro.targetId;
            });;
        if (iter == _descSortedArray.end())
        {
            return false;
        }

        _descSortedArray.erase(iter);

        return true;
    }

    void MonsterAggroComponent::RemovePrimary()
    {
        _descSortedArray.erase(_descSortedArray.begin());
    }

    void MonsterAggroComponent::RemoveGarbage()
    {
        const game_time_point_type now = GameTimeService::Now();
        const std::chrono::milliseconds memoryTime(_data.memoryTime);

        for (auto iter = _descSortedArray.begin(); iter != _descSortedArray.end(); )
        {
            if (now - iter->lastUpdateTimePoint > memoryTime)
            {
                iter = _descSortedArray.erase(iter);
            }
            else
            {
                ++iter;
            }
        }
    }

    void MonsterAggroComponent::AddByDamage(game_entity_id_type targetId, int32_t damage)
    {
        if (damage <= 0)
        {
            return;
        }

        if (auto iter = std::ranges::find_if(_descSortedArray, [targetId](const Aggro& aggro)
            {
                return targetId == aggro.targetId;
            });
            iter != _descSortedArray.end())
        {
            AddValue(iter, damage, true);

            return;
        }

        if (std::ssize(_descSortedArray) == _data.maxEnemy)
        {
            _descSortedArray.pop_back();
        }

        Aggro aggro;
        aggro.lastUpdateTimePoint = GameTimeService::Now();
        aggro.targetId = targetId;
        aggro.damage = damage;
        aggro.value = damage;

        const auto pos = std::ranges::lower_bound(_descSortedArray, aggro, CompareAggroValue);
        _descSortedArray.insert(pos, aggro);
    }

    void MonsterAggroComponent::AddValue(game_entity_id_type targetId, int32_t value)
    {
        if (value == 0)
        {
            return;
        }

        if (auto iter = std::ranges::find_if(_descSortedArray, [targetId](const Aggro& aggro)
            {
                return targetId == aggro.targetId;
            });
            iter != _descSortedArray.end())
        {
            AddValue(iter, value, false);

            return;
        }

        if (std::ssize(_descSortedArray) == _data.maxEnemy)
        {
            RemoveGarbage();

            if (std::ssize(_descSortedArray) == _data.maxEnemy)
            {
                _descSortedArray.pop_back();
            }
        }

        Aggro aggro;
        aggro.lastUpdateTimePoint = GameTimeService::Now();
        aggro.targetId = targetId;
        aggro.value = value;

        const auto pos = std::ranges::lower_bound(_descSortedArray, aggro, CompareAggroValue);
        _descSortedArray.insert(pos, aggro);
    }

    auto MonsterAggroComponent::GetPrimaryTarget() const -> std::optional<game_entity_id_type>
    {
        if (_descSortedArray.empty())
        {
            return std::nullopt;
        }

        return _descSortedArray[0].targetId;
    }

    void MonsterAggroComponent::AddValue(std::vector<Aggro>::iterator iter, int32_t value, bool damage)
    {
        iter->lastUpdateTimePoint = GameTimeService::Now();
        iter->value += value;

        if (damage)
        {
            iter->damage += value;
        }

        if (value > 0)
        {
            while (iter != _descSortedArray.begin())
            {
                auto prev = std::prev(iter);

                if (CompareAggroValue(*iter, *prev))
                {
                    std::iter_swap(iter, prev);
                }
                else
                {
                    break;
                }

                iter = prev;
            }
        }
        else
        {
            auto prev = iter;
            for (auto next = std::next(prev); next != _descSortedArray.end(); prev = next++)
            {
                if (CompareAggroValue(*prev, *next))
                {
                    std::iter_swap(prev, next);
                }
                else
                {
                    break;
                }
            }
        }
    }

    bool MonsterAggroComponent::CompareAggroValue(const Aggro& lhs, const Aggro& rhs)
    {
        return lhs.value > rhs.value;
    }
}
