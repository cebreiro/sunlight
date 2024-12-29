#pragma once
#include "sl/generator/game/contents/server_command/server_command_interface.h"

namespace sunlight
{
    class ServerCommandSystem;
}

namespace sunlight
{
    class ServerCommandMonsterSpawn final : public ServerCommandT<int32_t>
    {
    public:
        explicit ServerCommandMonsterSpawn(ServerCommandSystem& system);

        auto GetName() const -> std::string_view override;
        auto GetRequiredGmLevel() const -> int8_t override;

        bool Execute(GamePlayer& player, int32_t mobId) const override;

    private:
        ServerCommandSystem& _system;
    };

    class ServerCommandMonsterKill final : public ServerCommandT<>
    {
    public:
        explicit ServerCommandMonsterKill(ServerCommandSystem& system);

        auto GetName() const -> std::string_view override;
        auto GetRequiredGmLevel() const -> int8_t override;

        bool Execute(GamePlayer& player) const override;

    private:
        ServerCommandSystem& _system;
    };

    class ServerCommandMonsterRemove final : public ServerCommandT<>
    {
    public:
        explicit ServerCommandMonsterRemove(ServerCommandSystem& system);

        auto GetName() const -> std::string_view override;
        auto GetRequiredGmLevel() const -> int8_t override;

        bool Execute(GamePlayer& player) const override;

    private:
        ServerCommandSystem& _system;
    };

    class ServerCommandMonsterAISuspend final : public ServerCommandT<>
    {
    public:
        explicit ServerCommandMonsterAISuspend(ServerCommandSystem& system);

        auto GetName() const -> std::string_view override;
        auto GetRequiredGmLevel() const -> int8_t override;

        bool Execute(GamePlayer& player) const override;

    private:
        ServerCommandSystem& _system;
    };

    class ServerCommandMonsterAIResume final : public ServerCommandT<>
    {
    public:
        explicit ServerCommandMonsterAIResume(ServerCommandSystem& system);

        auto GetName() const -> std::string_view override;
        auto GetRequiredGmLevel() const -> int8_t override;

        bool Execute(GamePlayer& player) const override;

    private:
        ServerCommandSystem& _system;
    };

    class ServerCommandMonsterComeHere final : public ServerCommandT<>
    {
    public:
        explicit ServerCommandMonsterComeHere(ServerCommandSystem& system);

        auto GetName() const -> std::string_view override;
        auto GetRequiredGmLevel() const -> int8_t override;

        bool Execute(GamePlayer& player) const override;

    private:
        ServerCommandSystem& _system;
    };
}
