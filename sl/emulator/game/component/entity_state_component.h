#pragma once
#include "sl/emulator/game/component/game_component.h"
#include "sl/emulator/game/contants/state/game_entity_state.h"

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
