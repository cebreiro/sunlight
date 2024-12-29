#include "entity_immune_component.h"

namespace sunlight
{
    bool EntityImmuneComponent::Contains(ImmuneType immuneType) const
    {
        return !GetList(immuneType).empty();
    }

    bool EntityImmuneComponent::Rand(ImmuneType immuneType, std::mt19937& mt) const
    {
        std::uniform_int_distribution dist(0, 100);

        for (const int32_t percentage : GetList(immuneType) | std::views::elements<1>)
        {
            if (percentage >= 100)
            {
                return true;
            }

            if (dist(mt) <= percentage)
            {
                return true;
            }
        }

        return false;
    }

    void EntityImmuneComponent::Add(const ImmuneOrigin& origin, int32_t percentage)
    {
        MutableList(origin.GetImmuneType()).emplace_back(origin, percentage);
    }

    bool EntityImmuneComponent::Remove(const ImmuneOrigin& origin)
    {
        std::vector<std::pair<ImmuneOrigin, int32_t>>& immuneOrigins = MutableList(origin.GetImmuneType());

        const auto iter = std::ranges::find_if(immuneOrigins, [&origin](const auto& pair)
            {
                return pair.first == origin;
            });
        if (iter == immuneOrigins.end())
        {
            return false;
        }

        immuneOrigins.erase(iter);

        return true;
    }

    auto EntityImmuneComponent::MutableList(ImmuneType type) -> std::vector<std::pair<ImmuneOrigin, int32_t>>&
    {
        const int32_t index = static_cast<int32_t>(type);
        assert(index >= 0 && index < static_cast<int32_t>(std::ssize(_immuneOrigins)));

        return _immuneOrigins[index];
    }

    auto EntityImmuneComponent::GetList(ImmuneType type) const -> const std::vector<std::pair<ImmuneOrigin, int32_t>>&
    {
        const int32_t index = static_cast<int32_t>(type);
        assert(index >= 0 && index < static_cast<int32_t>(std::ssize(_immuneOrigins)));

        return _immuneOrigins[index];
    }
}
