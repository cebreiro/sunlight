-- Zone  : 202
-- Stage : 10020
-- Name  : 한스 (에이전트 전직 담당)
-- Nes   : 100086

return function (system, npc, player, sequence)

    local ngs = require("job_promote")

    local param = {
        stringBase = 2900,
        progressCheckQuestId = 1100,
        questId = 1104,
        jobId = 2210,
        requiredItemId = 5010011,
        monsterId = 101,
        monsterCount = 5,
        probability = 1000000,
        isValidJob = ngs.isRanger
    }

    ngs.handleScript_JobChangeSupervisor(system, npc, player, sequence, param)

end
