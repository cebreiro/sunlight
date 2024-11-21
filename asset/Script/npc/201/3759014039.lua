-- Zone  : 201
-- Stage : 10014
-- Name  : 제시 (권법가 전직 담당)
-- Nes   : 100029

return function (system, npc, player, sequence)

    local param = {
        stringBase = 1900,
        questId = 1004,
        jobId = 2103,
        requiredItemId = 5050002,
        monsterId = 11802,
        monsterCount = 10,
        probability = 1000000
    }

    local ngs = require("job_promote_novice_fighter")

    ngs.handleScript_JobChangeSupervisor(system, npc, player, sequence, param)

end
