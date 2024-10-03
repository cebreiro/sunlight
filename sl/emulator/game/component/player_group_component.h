#pragma once
#include "sl/emulator/game/contants/group/game_group_type.h"
#include "sl/emulator/game/component/game_component.h"

namespace sunlight
{
    class PlayerGroupComponent final : public GameComponent
    {
    public:
        bool HasGroup() const;

        void Clear();

        auto GetGroupType() const -> GameGroupType;
        auto GetGroupId() const -> int32_t;

        void SetGroupId(std::optional<int32_t> groupId);
        void SetGroupType(GameGroupType type);

    private:
        bool _hasGroup = false;

        std::optional<int32_t> _groupId = std::nullopt;
        GameGroupType _groupType = GameGroupType::Null;
    };
}
