return function (system, npc, player, sequence)

    local param = {
        stringBase = 1700,
        progressCheckQuestId = 1000,
        questId = 1002,
        jobId = 2101,
        requiredItemId = 5050002,
        monsterId = 12901,
        monsterCount = 10,
        probability = 1000000
    }

    local ngs = require("job_promote")

    ngs.handleScript_JobChangeSupervisor(system, npc, player, sequence, param)

end
