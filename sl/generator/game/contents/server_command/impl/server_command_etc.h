#pragma once
#include "sl/generator/game/contents/server_command/server_command_interface.h"

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

    class ServerCommandGoldAdd final : public ServerCommandT<int32_t>
    {
    public:
        explicit ServerCommandGoldAdd(ServerCommandSystem& system);

        auto GetName() const -> std::string_view override;
        auto GetRequiredGmLevel() const -> int8_t override;

    public:
        bool Execute(GamePlayer& player, int32_t value) const override;

    private:
        ServerCommandSystem& _system;
    };

}
