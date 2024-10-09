#pragma once
#include "sl/emulator/game/contents/server_command/server_command_interface.h"

namespace sunlight
{
    class ServerCommandSystem;
}


namespace sunlight
{
    class ServerCommandDebugServer final : public ServerCommandT<int32_t, int32_t>
    {
    public:
        ServerCommandDebugServer(ServerCommandSystem& system);

        auto GetName() const->std::string_view override;
        auto GetRequiredGmLevel() const->int8_t override;

    public:
        bool Execute(GamePlayer& player, int32_t type, int32_t value) const override;

    private:
        ServerCommandSystem& _system;
    };

    class ServerCommandDebugServerOff final : public ServerCommandT<>
    {
    public:
        ServerCommandDebugServerOff(ServerCommandSystem& system);

        auto GetName() const->std::string_view override;
        auto GetRequiredGmLevel() const->int8_t override;

    public:
        bool Execute(GamePlayer& player) const override;

    private:
        ServerCommandSystem& _system;
    };
}
