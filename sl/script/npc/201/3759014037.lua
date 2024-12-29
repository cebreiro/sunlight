return function (system, npc, player, sequence)

    -- TODO: 겸직 로직 만들기

    local ngs = require("job_promote")

    local param = {
        stringBase = 1100,
        progressCheckQuestId = 1000,
        isValidJob = ngs.isFighter,
        jobChangeQuestTable = { 1001, 1002, 1003, 1004 }
    }

    ngs.handleScript_JobChangGuildMaster(system, npc, player, sequence, param)
    
end
