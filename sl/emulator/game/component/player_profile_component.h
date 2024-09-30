#pragma once
#include "sl/emulator/game/component/game_component.h"

namespace sunlight
{
    class PlayerProfileComponent final : public GameComponent
    {
    public:
        bool IsPrivate() const;


        void SetPrivate(bool value);

    private:
        bool _private = false;
    };
}
