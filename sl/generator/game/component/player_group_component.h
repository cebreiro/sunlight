#pragma once
#include "sl/generator/game/contents/group/game_group_state.h"
#include "sl/generator/game/component/game_component.h"

namespace sunlight
{
    class PlayerGroupComponent final : public GameComponent
    {
    public:
        bool HasGroup() const;

        void Clear();

        auto GetGroupType() const -> GameGroupType;
        auto GetGroupId() const -> int32_t;
        auto GetGroupState() const -> const GameGroupState&;

        void SetGroupId(std::optional<int32_t> groupId);
        void SetGroupType(GameGroupType type);
        void SetGroupState(const GameGroupState& state);

    private:
        bool _hasGroup = false;

        std::optional<int32_t> _groupId = std::nullopt;
        GameGroupType _groupType = GameGroupType::Null;
        GameGroupState _state = {};
    };
}
