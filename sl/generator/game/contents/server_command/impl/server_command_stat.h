#pragma once
#include "sl/generator/game/contents/server_command/server_command_interface.h"

namespace sunlight
{
    class ServerCommandSystem;
}

namespace sunlight
{
    class ServerCommandStatHPSet final : public ServerCommandT<int32_t>
    {
    public:
        explicit ServerCommandStatHPSet(ServerCommandSystem& system);

        auto GetName() const -> std::string_view override;
        auto GetRequiredGmLevel() const -> int8_t override;

    public:
        bool Execute(GamePlayer& player, int32_t value) const override;

    private:
        ServerCommandSystem& _system;
    };

    class ServerCommandStatSPSet final : public ServerCommandT<int32_t>
    {
    public:
        explicit ServerCommandStatSPSet(ServerCommandSystem& system);

        auto GetName() const -> std::string_view override;
        auto GetRequiredGmLevel() const -> int8_t override;

    public:
        bool Execute(GamePlayer& player, int32_t value) const override;

    private:
        ServerCommandSystem& _system;
    };

    class ServerCommandStatShow final : public ServerCommandT<>
    {
    public:
        ServerCommandStatShow() = default;

        auto GetName() const -> std::string_view override;
        auto GetRequiredGmLevel() const -> int8_t override;

    public:
        bool Execute(GamePlayer& player) const override;
    };
}
