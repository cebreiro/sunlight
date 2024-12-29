
return function (system, npc, player, sequence)

    local ngs = require("job_promote")

    local param = {
        stringBase = 2600,
        progressCheckQuestId = 1100,
        questId = 1101,
        jobId = 2208,
        requiredItemId = 5050004,
        monsterId = 11801,
        monsterCount = 10,
        probability = 1000000,
        isValidJob = ngs.isRanger
    }

    ngs.handleScript_JobChangeSupervisor(system, npc, player, sequence, param)

end
