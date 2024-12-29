return function (system, npc, player, sequence)

    local ngs = require("job_promote")

    local param = {
        stringBase = 1600,
        progressCheckQuestId = 1000,
        questId = 1001,
        jobId = 2100,
        requiredItemId = 5050001,
        monsterId = 11802,
        monsterCount = 10,
        probability = 1000000,
        isValidJob = ngs.isFighter
    }

    ngs.handleScript_JobChangeSupervisor(system, npc, player, sequence, param)

end
