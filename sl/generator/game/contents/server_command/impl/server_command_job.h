#pragma once
#include "sl/generator/game/contents/server_command/server_command_interface.h"

namespace sunlight
{
    class ServerCommandSystem;
}

namespace sunlight
{
    class ServerCommandJobPromote final : public ServerCommandT<int32_t>
    {
    public:
        explicit ServerCommandJobPromote(ServerCommandSystem& system);

        auto GetName() const -> std::string_view override;
        auto GetRequiredGmLevel() const -> int8_t override;

    public:
        bool Execute(GamePlayer& player, int32_t advanced) const override;

    private:
        ServerCommandSystem& _system;
    };
}
