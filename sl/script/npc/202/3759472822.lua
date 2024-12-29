
return function (system, npc, player, sequence)

    local ngs = require("job_promote")

    local param = {
        stringBase = 33700,
        progressCheckQuestId = 1200,
        questId = 1202,
        jobId = 2305,
        requiredItemId = -1,
        monsterId = 10201,
        monsterCount = 10,
        probability = 1000000,
        isValidJob = ngs.isMagician
    }

    ngs.handleScript_JobChangeSupervisor(system, npc, player, sequence, param)

end
