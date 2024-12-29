#pragma once
#include "sl/generator/game/contents/server_command/server_command_interface.h"

namespace sunlight
{
    class ServerCommandSystem;
}

namespace sunlight
{
    class ServerCommandDebugServer final : public ServerCommandT<int32_t, int32_t>
    {
    public:
        ServerCommandDebugServer(ServerCommandSystem& system);

        auto GetName() const->std::string_view override;
        auto GetRequiredGmLevel() const->int8_t override;

    public:
        bool Execute(GamePlayer& player, int32_t type, int32_t value) const override;

    private:
        ServerCommandSystem& _system;
    };

    class ServerCommandDebugServerOff final : public ServerCommandT<>
    {
    public:
        ServerCommandDebugServerOff(ServerCommandSystem& system);

        auto GetName() const->std::string_view override;
        auto GetRequiredGmLevel() const->int8_t override;

    public:
        bool Execute(GamePlayer& player) const override;

    private:
        ServerCommandSystem& _system;
    };

    class ServerCommandDebugPlayerHP final : public ServerCommandT<>
    {
    public:
        ServerCommandDebugPlayerHP(ServerCommandSystem& system);

        auto GetName() const -> std::string_view override;
        auto GetRequiredGmLevel() const -> int8_t override;

    public:
        bool Execute(GamePlayer& player) const override;

    private:
        void DebugNotify(GamePlayer& player) const;

    private:
        ServerCommandSystem& _system;
    };

    class ServerCommandDebugPlayerSP final : public ServerCommandT<>
    {
    public:
        ServerCommandDebugPlayerSP(ServerCommandSystem& system);

        auto GetName() const -> std::string_view override;
        auto GetRequiredGmLevel() const -> int8_t override;

    public:
        bool Execute(GamePlayer& player) const override;

    private:
        void DebugNotify(GamePlayer& player) const;

    private:
        ServerCommandSystem& _system;
    };

    class ServerCommandDebugTile final : public ServerCommandT<>
    {
    public:
        explicit ServerCommandDebugTile(ServerCommandSystem& system);

        auto GetName() const -> std::string_view override;
        auto GetRequiredGmLevel() const -> int8_t override;

    public:
        bool Execute(GamePlayer& player) const override;

    private:
        ServerCommandSystem& _system;
    };

    class ServerCommandDebugPathFind final : public ServerCommandT<float, float>
    {
    public:
        explicit ServerCommandDebugPathFind(ServerCommandSystem& system);

        auto GetName() const -> std::string_view override;
        auto GetRequiredGmLevel() const -> int8_t override;

    public:
        bool Execute(GamePlayer& player, float x, float y) const override;

    private:
        ServerCommandSystem& _system;
    };

    class ServerCommandDebugPathFindRaw final : public ServerCommandT<float, float>
    {
    public:
        explicit ServerCommandDebugPathFindRaw(ServerCommandSystem& system);

        auto GetName() const -> std::string_view override;
        auto GetRequiredGmLevel() const -> int8_t override;

    public:
        bool Execute(GamePlayer& player, float x, float y) const override;

    private:
        ServerCommandSystem& _system;
    };

    class ServerCommandDebugPathBlocked final : public ServerCommandT<float, float>
    {
    public:
        explicit ServerCommandDebugPathBlocked(ServerCommandSystem& system);

        auto GetName() const -> std::string_view override;
        auto GetRequiredGmLevel() const -> int8_t override;

    public:
        bool Execute(GamePlayer& player, float x, float y) const override;

    private:
        ServerCommandSystem& _system;
    };

    class ServerCommandDebugMonsterPosition final : public ServerCommandT<>
    {
    public:
        explicit ServerCommandDebugMonsterPosition(ServerCommandSystem& system);

        auto GetName() const -> std::string_view override;
        auto GetRequiredGmLevel() const -> int8_t override;

    public:
        bool Execute(GamePlayer& player) const override;

    private:
        ServerCommandSystem& _system;
    };

    class ServerCommandDebugStringTable final : public ServerCommandT<int32_t>
    {
    public:
        ServerCommandDebugStringTable() = default;

        auto GetName() const -> std::string_view override;
        auto GetRequiredGmLevel() const -> int8_t override;

    public:
        bool Execute(GamePlayer& player, int32_t index) const override;
    };


}
