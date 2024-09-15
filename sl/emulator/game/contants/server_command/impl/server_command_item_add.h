#pragma once
#include "sl/emulator/game/contants/server_command/server_command_interface.h"

namespace sunlight
{
    class ServerCommandSystem;
}

namespace sunlight
{
    class ServerCommandItemAdd final : public ServerCommandT<int32_t, int32_t>
    {
    public:
        explicit ServerCommandItemAdd(ServerCommandSystem& system);

        bool Execute(GamePlayer& player, int32_t itemId, int32_t quantity) const override;

        auto GetName() const -> std::string_view override;

    private:
        ServerCommandSystem& _system;
        const ServiceLocator& _serviceLocator;
    };
}
