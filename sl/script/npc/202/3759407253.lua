
return function (system, npc, player, sequence)

    local ngs = require("job_promote")

    local param = {
        stringBase = 2700,
        progressCheckQuestId = 1100,
        questId = 1102,
        jobId = 2209,
        requiredItemId = 5050004,
        monsterId = 11801,
        monsterCount = 10,
        probability = 1000000,
        isValidJob = ngs.isRanger
    }

    ngs.handleScript_JobChangeSupervisor(system, npc, player, sequence, param)

end
