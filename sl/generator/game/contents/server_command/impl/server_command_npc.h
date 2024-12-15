#pragma once
#include "sl/generator/game/contents/server_command/server_command_interface.h"

namespace sunlight
{
    class ServerCommandSystem;
}

namespace sunlight
{
    class ServerCommandNPCShopRoll final : public ServerCommandT<>
    {
    public:
        explicit ServerCommandNPCShopRoll(ServerCommandSystem& system);

        auto GetName() const->std::string_view override;
        auto GetRequiredGmLevel() const->int8_t override;

        bool Execute(GamePlayer& player) const override;

    private:
        ServerCommandSystem& _system;
    };

    class ServerCommandNPCShopDebugSlot final : public ServerCommandT<int32_t>
    {
    public:
        explicit ServerCommandNPCShopDebugSlot(ServerCommandSystem& system);

        auto GetName() const->std::string_view override;
        auto GetRequiredGmLevel() const->int8_t override;

        bool Execute(GamePlayer& player, int32_t page) const override;

    private:
        ServerCommandSystem& _system;
    };
}
