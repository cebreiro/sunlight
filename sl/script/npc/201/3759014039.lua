return function (system, npc, player, sequence)

    local ngs = require("job_promote")

    local param = {
        stringBase = 1900,
        progressCheckQuestId = 1000,
        questId = 1004,
        jobId = 2103,
        requiredItemId = 5050002,
        monsterId = 11802,
        monsterCount = 10,
        probability = 1000000,
        isValidJob = ngs.isRanger
    }

    ngs.handleScript_JobChangeSupervisor(system, npc, player, sequence, param)

end
