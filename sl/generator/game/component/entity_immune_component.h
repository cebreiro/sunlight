#pragma once
#include "sl/generator/game/component/game_component.h"
#include "sl/generator/game/contents/immune/immune_origin.h"

namespace sunlight
{
    class EntityImmuneComponent final : public GameComponent
    {
    public:
        bool Contains(ImmuneType immuneType) const;
        bool Rand(ImmuneType immuneType, std::mt19937& mt) const;

        void Add(const ImmuneOrigin& origin, int32_t percentage);
        bool Remove(const ImmuneOrigin& origin);

    private:
        auto MutableList(ImmuneType type) -> std::vector<std::pair<ImmuneOrigin, int32_t>>&;
        auto GetList(ImmuneType type) const -> const std::vector<std::pair<ImmuneOrigin, int32_t>>&;

    private:
        std::array<std::vector<std::pair<ImmuneOrigin, int32_t>>, ImmuneTypeMeta::GetSize()> _immuneOrigins;
    };
}
