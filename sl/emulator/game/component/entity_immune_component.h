#pragma once
#include "sl/emulator/game/component/game_component.h"
#include "sl/emulator/game/contents/immune/immune_origin.h"

namespace sunlight
{
    class EntityImmuneComponent final : public GameComponent
    {
    public:
        bool Contains(ImmuneType immuneType) const;

        void Add(const ImmuneOrigin& origin);
        bool Remove(const ImmuneOrigin& origin);

    private:
        std::unordered_multimap<ImmuneType, ImmuneOrigin> _immuneTypes;
    };
}
