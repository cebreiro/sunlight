#pragma once
#include "sl/emulator/game/component/game_component.h"
#include "sl/emulator/game/entity/game_entity_id_type.h"

namespace sunlight
{
    class PlayerNPCScriptComponent final : public GameComponent
    {
    public:
        bool HasTargetNPC() const;

        void Clear();

        auto GetTargetNPCId() const -> game_entity_id_type;
        auto GetSequence() const -> int32_t;
        auto GetSelection() const -> int32_t;

        void SetTargetNPCId(game_entity_id_type id);
        void SetSequence(int32_t value);
        void SetSelection(int32_t selection);

    private:
        std::optional<game_entity_id_type> _targetNPCId = std::nullopt;
        int32_t _sequence = 0;
        int32_t _selection = 0;
    };
}
