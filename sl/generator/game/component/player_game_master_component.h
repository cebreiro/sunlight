#pragma once
#include "sl/generator/game/component/game_component.h"

namespace sunlight
{
    class PlayerGameMasterComponent final : public GameComponent
    {
    public:
        auto GetDamage() const -> std::optional<int32_t>;

        void SetDamage(int32_t damage);
        void ClearDamage();

    private:
        std::optional<int32_t> _damage = std::nullopt;
    };
}
