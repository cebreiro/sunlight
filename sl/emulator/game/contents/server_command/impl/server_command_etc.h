#pragma once
#include "sl/emulator/game/contents/server_command/server_command_interface.h"

namespace sunlight
{
    class ServerCommandSystem;
}

namespace sunlight
{
    class ServerCommandSuicide final : public ServerCommandT<>
    {
    public:
        explicit ServerCommandSuicide(ServerCommandSystem& system);

        auto GetName() const -> std::string_view override;
        auto GetRequiredGmLevel() const -> int8_t override;

    public:
        bool Execute(GamePlayer& player) const override;

    private:
        ServerCommandSystem& _system;
    };

}
