#pragma once
#include "sl/generator/game/component/game_component.h"
#include "sl/generator/game/contents/state/game_entity_state.h"

namespace sunlight
{
    class EntityStateComponent final : public GameComponent
    {
    public:
        auto GetState() const -> const GameEntityState&;

        void SetState(const GameEntityState& state);

    private:
        GameEntityState _state;
    };
}
