#include "player_npc_script_component.h"

namespace sunlight
{
    bool PlayerNPCScriptComponent::HasTargetNPC() const
    {
        return _targetNPCId.has_value();
    }

    void PlayerNPCScriptComponent::Clear()
    {
        _targetNPCId.reset();
        _sequence = 0;
        _selection = 0;
        _state = 0;
    }

    auto PlayerNPCScriptComponent::GetTargetNPCId() const -> game_entity_id_type
    {
        assert(_targetNPCId.has_value());

        return *_targetNPCId;
    }

    auto PlayerNPCScriptComponent::GetSequence() const -> int32_t
    {
        return _sequence;
    }

    auto PlayerNPCScriptComponent::GetSelection() const -> int32_t
    {
        return _selection;
    }

    auto PlayerNPCScriptComponent::GetState() const -> int32_t
    {
        return _state;
    }

    void PlayerNPCScriptComponent::SetTargetNPCId(game_entity_id_type id)
    {
        _targetNPCId = id;
    }

    void PlayerNPCScriptComponent::SetSequence(int32_t value)
    {
        _sequence = value;
    }

    void PlayerNPCScriptComponent::SetSelection(int32_t selection)
    {
        _selection = selection;
    }

    void PlayerNPCScriptComponent::SetState(int32_t state)
    {
        _state = state;
    }
}
