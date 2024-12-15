#pragma once
#include "sl/generator/game/contents/server_command/server_command_param_reader.h"

namespace sunlight
{
    class GamePlayer;
    class ServerCommandParamReader;
}

namespace sunlight
{
    class IServerCommand
    {
    public:
        virtual ~IServerCommand() = default;

        virtual bool HandleCommand(GamePlayer& player, const ServerCommandParamReader& reader) const = 0;
        virtual auto GetName() const -> std::string_view = 0;
        virtual auto GetRequiredGmLevel() const -> int8_t = 0;
    };


    template <typename... TParams>
    class ServerCommandT : public IServerCommand
    {
    public:
        bool HandleCommand(GamePlayer& player, const ServerCommandParamReader& reader) const final
        {
            using tuple_type = std::tuple<TParams...>;

            tuple_type tuple;

            const auto parse = [&reader]<typename T>(T& value, int64_t index) -> bool
                {
                    if constexpr (std::is_same_v<T, std::string>)
                    {
                        reader.Get(index, value);

                        return true;
                    }
                    else
                    {
                        return reader.TryParse<T>(index, value);
                    }
                };

            const auto build = [&]<size_t...I>(tuple_type & tuple, std::index_sequence<I...>) -> bool
                {
                    return (... && parse(std::get<I>(tuple), static_cast<int64_t>(I)));
                };

            const bool result = build(tuple, std::make_index_sequence<sizeof...(TParams)>());

            if (!result)
            {
                return false;
            }

            return std::apply(std::bind_front(&ServerCommandT::Execute, this, std::ref(player)), std::move(tuple));
        }

        virtual bool Execute(GamePlayer& player, TParams... params) const = 0;
    };
}
