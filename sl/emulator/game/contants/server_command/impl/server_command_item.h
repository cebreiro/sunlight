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

        auto GetName() const -> std::string_view override;
        auto GetRequiredGmLevel() const -> int8_t override;

    public:
        bool Execute(GamePlayer& player, int32_t itemId, int32_t quantity) const override;

    private:
        ServerCommandSystem& _system;
    };

    class ServerCommandItemGain final : public ServerCommandT<int32_t, int32_t>
    {
    public:
        explicit ServerCommandItemGain(ServerCommandSystem& system);

        auto GetName() const->std::string_view override;
        auto GetRequiredGmLevel() const->int8_t override;

    public:
        bool Execute(GamePlayer& player, int32_t itemId, int32_t quantity) const override;

    private:
        ServerCommandSystem& _system;
    };
}
