#pragma once
#include "sl/generator/game/contents/server_command/server_command_interface.h"

namespace sunlight
{
    class ServerCommandSystem;
}

namespace sunlight
{
    class ServerCommandQueryNesCaller final : public ServerCommandT<int32_t>
    {
    public:
        explicit ServerCommandQueryNesCaller(ServerCommandSystem& system);
        
        auto GetName() const -> std::string_view override;
        auto GetRequiredGmLevel() const -> int8_t override;

        bool Execute(GamePlayer& player, int32_t scriptId) const override;

    private:
        ServerCommandSystem& _system;
    };
}
