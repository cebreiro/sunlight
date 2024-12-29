#pragma once
#include "shared/type/runtime_type_id.h"
#include "sl/generator/game/component/game_component_id.h"

namespace sunlight
{
    class GameComponent
    {
    public:
        virtual ~GameComponent() = default;

        template <typename T> requires std::derived_from<T, GameComponent>
        static auto GetClassId() -> game_component_id_type;
    };

    template <typename T> requires std::derived_from<T, GameComponent>
    auto GameComponent::GetClassId() -> game_component_id_type
    {
        const int64_t value = RuntimeTypeId<GameComponent>::Get<T>();

        using value_type = game_component_id_type::value_type;
        static_assert(std::is_same_v<int64_t, value_type>);

        return game_component_id_type(value);
    }
}
