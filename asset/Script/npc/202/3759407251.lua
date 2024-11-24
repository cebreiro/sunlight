-- Zone  : 202
-- Stage : 10020
-- Name  : 레프 (트레저헌터 전직 담당)
-- Nes   : 100085

return function (system, npc, player, sequence)

    local ngs = require("job_promote")

    local param = {
        stringBase = 2800,
        progressCheckQuestId = 1100,
        questId = 1103,
        jobId = 2211,
        requiredItemId = 5050005,
        monsterId = 11901,
        monsterCount = 10,
        probability = 1000000,
        isValidJob = ngs.isRanger
    }

    ngs.handleScript_JobChangeSupervisor(system, npc, player, sequence, param)

end
