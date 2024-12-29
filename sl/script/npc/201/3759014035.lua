return function (system, npc, player, sequence)

    local ngs = require("job_promote")

    local param = {
        stringBase = 1800,
        progressCheckQuestId = 1000,
        questId = 1003,
        jobId = 2102,
        requiredItemId = 5050002,
        monsterId = 12901,
        monsterCount = 10,
        probability = 1000000,
        isValidJob = ngs.isFighter
    }

    ngs.handleScript_JobChangeSupervisor(system, npc, player, sequence, param)

end
