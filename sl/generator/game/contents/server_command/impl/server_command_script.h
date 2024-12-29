#pragma once
#include "sl/generator/game/contents/server_command/server_command_interface.h"

namespace sunlight
{
    class ServerCommandSystem;
}

namespace sunlight
{
    class ServerCommandScriptReload final : public ServerCommandT<>
    {
    public:
        explicit ServerCommandScriptReload(ServerCommandSystem& system);

        auto GetName() const -> std::string_view override;
        auto GetRequiredGmLevel() const -> int8_t override;

        bool Execute(GamePlayer& player) const override;

    private:
        ServerCommandSystem& _system;
    };

    class ServerCommandScriptExecute final : public ServerCommandT<std::string>
    {
    public:
        explicit ServerCommandScriptExecute(ServerCommandSystem& system);

        auto GetName() const -> std::string_view override;
        auto GetRequiredGmLevel() const -> int8_t override;

        bool Execute(GamePlayer& player, std::string name) const override;

    private:
        ServerCommandSystem& _system;
    };
}
