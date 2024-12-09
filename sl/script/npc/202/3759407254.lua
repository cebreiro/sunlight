-- Zone  : 202
-- Stage : 10020
-- Name  : 길드장 베놈
-- Nes   : 100060

return function (system, npc, player, sequence)

    -- TODO: 겸직 로직 만들기

    local ngs = require("job_promote")

    local param = {
        stringBase = 2100,
        progressCheckQuestId = 1100,
        isValidJob = ngs.isRanger,
        jobChangeQuestTable = { 1101, 1102, 1103, 1104 }
    }

    ngs.handleScript_JobChangGuildMaster(system, npc, player, sequence, param)
    
end
