-- Zone  : 202
-- Stage : 10021
-- Name  : 길드장 디안나
-- Nes   : 100088

return function (system, npc, player, sequence)

    -- TODO: 겸직 로직 만들기

    local ngs = require("job_promote")

    local param = {
        stringBase = 33100,
        progressCheckQuestId = 1200,
        isValidJob = ngs.isMagician,
        jobChangeQuestTable = { 1201, 1202, 1203, 1204 }
    }

    ngs.handleScript_JobChangGuildMaster(system, npc, player, sequence, param)
    
end
