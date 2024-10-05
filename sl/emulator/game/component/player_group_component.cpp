#include "player_group_component.h"

namespace sunlight
{
    bool PlayerGroupComponent::HasGroup() const
    {
        return _groupId.has_value();
    }

    void PlayerGroupComponent::Clear()
    {
        _hasGroup = false;
        _groupId = std::nullopt;
        _groupType = GameGroupType::Null;

        _state.title.clear();
        _state.groupId = 0;
        _state.groupType = GameGroupType::Null;
        _state.type = 0;
    }

    auto PlayerGroupComponent::GetGroupType() const -> GameGroupType
    {
        return _groupType;
    }

    auto PlayerGroupComponent::GetGroupId() const -> int32_t
    {
        assert(_groupId.has_value());

        return *_groupId;
    }

    auto PlayerGroupComponent::GetGroupState() const -> const GameGroupState&
    {
        return _state;
    }

    void PlayerGroupComponent::SetGroupId(std::optional<int32_t> groupId)
    {
        _groupId = groupId;
    }

    void PlayerGroupComponent::SetGroupType(GameGroupType type)
    {
        _groupType = type;
    }

    void PlayerGroupComponent::SetGroupState(const GameGroupState& state)
    {
        _state = state;
    }
}
