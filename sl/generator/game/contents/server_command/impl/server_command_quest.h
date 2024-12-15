#pragma once
#include "sl/generator/game/contents/server_command/server_command_interface.h"

namespace sunlight
{
    class ServerCommandSystem;
}

namespace sunlight
{
    class ServerCommandQuestAdd final : public ServerCommandT<int32_t>
    {
    public:
        explicit ServerCommandQuestAdd(ServerCommandSystem& system);
        
        auto GetName() const -> std::string_view override;
        auto GetRequiredGmLevel() const -> int8_t override;

        bool Execute(GamePlayer& player, int32_t questId) const override;

    private:
        ServerCommandSystem& _system;
    };

    class ServerCommandQuestStateSet final : public ServerCommandT<int32_t, int32_t>
    {
    public:
        explicit ServerCommandQuestStateSet(ServerCommandSystem& system);
        
        auto GetName() const -> std::string_view override;
        auto GetRequiredGmLevel() const -> int8_t override;

        bool Execute(GamePlayer& player, int32_t questId, int32_t state) const override;

    private:
        ServerCommandSystem& _system;
    };

    class ServerCommandQuestFlagSet final : public ServerCommandT<int32_t, int32_t, int32_t>
    {
    public:
        explicit ServerCommandQuestFlagSet(ServerCommandSystem& system);
        
        auto GetName() const -> std::string_view override;
        auto GetRequiredGmLevel() const -> int8_t override;

        bool Execute(GamePlayer& player, int32_t questId, int32_t index, int32_t value) const override;

    private:
        ServerCommandSystem& _system;
    };
}
