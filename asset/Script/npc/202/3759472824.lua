-- Zone  : 202
-- Stage : 10021
-- Name  : 패트린 (메이지 전직 담당)
-- Nes   : 100095

return function (system, npc, player, sequence)

    local ngs = require("job_promote")

    local param = {
        stringBase = 33600,
        progressCheckQuestId = 1200,
        questId = 1201,
        jobId = 2304,
        requiredItemId = -1,
        monsterId = 1201,
        monsterCount = 10,
        probability = 1000000,
        isValidJob = ngs.isMagician
    }

    ngs.handleScript_JobChangeSupervisor(system, npc, player, sequence, param)

end
