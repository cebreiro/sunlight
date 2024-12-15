#include "player_game_master_component.h"

namespace sunlight
{
    auto PlayerGameMasterComponent::GetDamage() const -> std::optional<int32_t>
    {
        return _damage;
    }

    void PlayerGameMasterComponent::SetDamage(int32_t damage)
    {
        _damage = damage;
    }

    void PlayerGameMasterComponent::ClearDamage()
    {
        _damage = std::nullopt;
    }
}
