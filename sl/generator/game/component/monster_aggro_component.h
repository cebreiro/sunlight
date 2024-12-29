#pragma once
#include "sl/generator/game/component/game_component.h"
#include "sl/generator/game/entity/game_entity_id_type.h"
#include "sl/generator/game/time/game_time.h"

namespace sunlight::sox
{
    struct MonsterAction;
}

namespace sunlight
{
    class MonsterAggroComponent final : public GameComponent
    {
    public:
        explicit MonsterAggroComponent(const sox::MonsterAction& data);

        bool Empty() const;

        bool Remove(game_entity_id_type targetId);
        void RemovePrimary();
        void RemoveGarbage();

        void AddByDamage(game_entity_id_type targetId, int32_t damage);
        void AddValue(game_entity_id_type targetId, int32_t value);

        auto GetPrimaryTarget() const -> std::optional<game_entity_id_type>;

    private:
        struct Aggro
        {
            game_entity_id_type targetId = {};
            game_time_point_type lastUpdateTimePoint = {};

            int32_t damage = 0;
            int32_t value = 0;
        };

        void AddValue(std::vector<Aggro>::iterator iter, int32_t value, bool damage);

        static bool CompareAggroValue(const Aggro& lhs, const Aggro& rhs);

    private:
        const sox::MonsterAction& _data;

        std::vector<Aggro> _descSortedArray;
    };
}
