#pragma once
#include "sl/generator/game/contents/server_command/server_command_interface.h"

namespace sunlight
{
    class ServerCommandSystem;
}

namespace sunlight
{
    class ServerCommandSkillAdd final : public ServerCommandT<int32_t, int32_t>
    {
    public:
        explicit ServerCommandSkillAdd(ServerCommandSystem& system);

        auto GetName() const -> std::string_view override;
        auto GetRequiredGmLevel() const -> int8_t override;

    public:
        bool Execute(GamePlayer& player, int32_t job, int32_t skillId) const override;

    private:
        ServerCommandSystem& _system;
    };
}
