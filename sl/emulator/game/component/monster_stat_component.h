#pragma once
#include "sl/emulator/game/component/game_component.h"
#include "sl/emulator/game/contents/stat/stat_value.h"

namespace sunlight::sox
{
    struct MonsterBase;
}

namespace sunlight
{
    class MonsterStatComponent final : public GameComponent
    {
    public:
        explicit MonsterStatComponent(const sox::MonsterBase& data);

        auto GetData() const -> const sox::MonsterBase&;
        auto GetHP() const -> StatValue;

        void SetHP(StatValue value);

    private:
        const sox::MonsterBase& _data;

        bool _dead = false;
        StatValue _hp = {};
    };
}
