-- Zone  : 201
-- Stage : 10014
-- Name  : 크란 (창기사 전직 담당)
-- Nes   : 100027

return function (system, npc, player, sequence)

    local param = {
        stringBase = 1800,
        questId = 1003,
        jobId = 2102,
        requiredItemId = 5050002,
        monsterId = 12901,
        monsterCount = 10,
        probability = 1000000
    }

    local ngs = require("job_promote_novice_fighter")

    ngs.handleScript_JobChangeSupervisor(system, npc, player, sequence, param)

end
