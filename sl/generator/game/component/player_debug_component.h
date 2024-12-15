#pragma once
#include "sl/generator/game/component/game_component.h"

namespace sunlight
{
    ENUM_CLASS(PlayerDebugType, int32_t,
        (HP)
        (SP)

        (Count)
    )

    class PlayerDebugComponent final : public GameComponent
    {
    public:
        bool IsSet(PlayerDebugType type) const;

        void Toggle(PlayerDebugType type);

    private:
        std::bitset<static_cast<size_t>(PlayerDebugType::Count)> _bitset = {};
    };
}
