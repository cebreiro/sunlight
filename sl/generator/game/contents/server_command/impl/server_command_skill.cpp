#include "server_command_skill.h"

#include "sl/generator/game/data/sox/job_reference.h"
#include "sl/generator/game/system/player_job_system.h"
#include "sl/generator/game/system/server_command_system.h"
#include "sl/generator/service/gamedata/gamedata_provide_service.h"

namespace sunlight
{
    ServerCommandSkillAdd::ServerCommandSkillAdd(ServerCommandSystem& system)
        : _system(system)
    {
    }

    auto ServerCommandSkillAdd::GetName() const -> std::string_view
    {
        return "skill_add";
    }

    auto ServerCommandSkillAdd::GetRequiredGmLevel() const -> int8_t
    {
        return 0;
    }

    bool ServerCommandSkillAdd::Execute(GamePlayer& player, int32_t job, int32_t skillId) const
    {
        if (job != static_cast<int32_t>(JobId::Any))
        {
            if (const sox::JobReference* jobData = _system.GetServiceLocator().Get<GameDataProvideService>().Get<sox::JobReferenceTable>().Find(job);
                !jobData)
            {
                return false;
            }
        }

        return _system.Get<PlayerJobSystem>().AddSkill(player, static_cast<JobId>(job), skillId);
    }
}
