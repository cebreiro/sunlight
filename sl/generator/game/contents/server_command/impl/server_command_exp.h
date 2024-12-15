#pragma once
#include "sl/generator/game/contents/server_command/server_command_interface.h"

namespace sunlight
{
    class ServerCommandSystem;
}

namespace sunlight
{
    class ServerCommandExpGain final : public ServerCommandT<int32_t>
    {
    public:
        explicit ServerCommandExpGain(ServerCommandSystem& system);

        auto GetName() const->std::string_view override;
        auto GetRequiredGmLevel() const->int8_t override;

    public:
        bool Execute(GamePlayer& player, int32_t value) const override;

    private:
        ServerCommandSystem& _system;
    };

    class ServerCommandExpCharacterGain final : public ServerCommandT<int32_t>
    {
    public:
        explicit ServerCommandExpCharacterGain(ServerCommandSystem& system);

        auto GetName() const->std::string_view override;
        auto GetRequiredGmLevel() const->int8_t override;

    public:
        bool Execute(GamePlayer& player, int32_t value) const override;

    private:
        ServerCommandSystem& _system;
    };

    class ServerCommandExpJobGain final : public ServerCommandT<int32_t>
    {
    public:
        explicit ServerCommandExpJobGain(ServerCommandSystem& system);

        auto GetName() const->std::string_view override;
        auto GetRequiredGmLevel() const->int8_t override;

    public:
        bool Execute(GamePlayer& player, int32_t value) const override;

    private:
        ServerCommandSystem& _system;
    };

}
