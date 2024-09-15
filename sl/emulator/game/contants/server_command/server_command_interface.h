#pragma once
#include "sl/emulator/game/contants/server_command/server_command_param_reader.h"

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
    };


    template <typename... TParams>
    class ServerCommandT : public IServerCommand
    {
    public:
        bool HandleCommand(GamePlayer& player, const ServerCommandParamReader& reader) const override
        {
            using tuple_type = std::tuple<TParams...>;

            tuple_type tuple;

            const auto build = [&reader]<size_t...I>(tuple_type & tuple, std::index_sequence<I...>) -> bool
                {
                    return (... && reader.TryParse<I, std::tuple_element_t<I, tuple_type>>(std::get<I>(tuple)));
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
