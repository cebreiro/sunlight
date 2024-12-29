#pragma once
#include "sl/generator/game/contents/server_command/server_command_interface.h"

namespace sunlight
{
    class ServerCommandSystem;
}

namespace sunlight
{
    class ServerCommandPacketObjectLeave final : public ServerCommandT<int32_t>
    {
    public:
        auto GetName() const->std::string_view override;
        auto GetRequiredGmLevel() const->int8_t override;

    public:
        bool Execute(GamePlayer& player, int32_t id) const override;
    };

    class ServerCommandPacketHairColorChange final : public ServerCommandT<int32_t>
    {
    public:
        explicit ServerCommandPacketHairColorChange(ServerCommandSystem& system);

        auto GetName() const -> std::string_view override;
        auto GetRequiredGmLevel() const -> int8_t override;

    public:
        bool Execute(GamePlayer& player, int32_t color) const override;

    private:
        ServerCommandSystem& _system;
    };

    class ServerCommandPacketSkinColorChange final : public ServerCommandT<int32_t>
    {
    public:
        explicit ServerCommandPacketSkinColorChange(ServerCommandSystem& system);

        auto GetName() const -> std::string_view override;
        auto GetRequiredGmLevel() const -> int8_t override;

    public:
        bool Execute(GamePlayer& player, int32_t color) const override;

    private:
        ServerCommandSystem& _system;
    };

    class ServerCommandPacketTest final : public ServerCommandT<>
    {
    public:
        auto GetName() const -> std::string_view override;
        auto GetRequiredGmLevel() const -> int8_t override;

    public:
        bool Execute(GamePlayer& player) const override;
    };
}
