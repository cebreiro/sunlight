
return function (system, npc, player, sequence)

    local ngs = require("job_promote")

    local param = {
        stringBase = 2100,
        progressCheckQuestId = 1100,
        isValidJob = ngs.isRanger,
        jobChangeQuestTable = { 1101, 1102, 1103, 1104 }
    }

    ngs.handleScript_JobChangGuildMaster(system, npc, player, sequence, param)
    
end
