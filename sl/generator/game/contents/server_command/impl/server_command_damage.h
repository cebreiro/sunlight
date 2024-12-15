#pragma once
#include "sl/generator/game/contents/server_command/server_command_interface.h"

namespace sunlight
{
    class ServerCommandSystem;
}


namespace sunlight
{
    class ServerCommandDamageSet final : public ServerCommandT<int32_t>
    {
    public:
        auto GetName() const->std::string_view override;
        auto GetRequiredGmLevel() const->int8_t override;

    public:
        bool Execute(GamePlayer& player, int32_t value) const override;
    };

    class ServerCommandDamageClear final : public ServerCommandT<>
    {
    public:
        auto GetName() const->std::string_view override;
        auto GetRequiredGmLevel() const->int8_t override;

    public:
        bool Execute(GamePlayer& player) const override;
    };
}
