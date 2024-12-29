#pragma once
#include "sl/generator/game/contents/server_command/server_command_interface.h"

namespace sunlight
{
    class ServerCommandSystem;
}

namespace sunlight
{
    class ServerCommandTeleport final : public ServerCommandT<int32_t, int32_t>
    {
    public:
        explicit ServerCommandTeleport(ServerCommandSystem& system);

        auto GetName() const->std::string_view override;
        auto GetRequiredGmLevel() const->int8_t override;

    public:
        bool Execute(GamePlayer& player, int32_t x, int32_t y) const override;

    private:
        ServerCommandSystem& _system;
    };
}
