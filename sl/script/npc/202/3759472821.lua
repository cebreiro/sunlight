return function (system, npc, player, sequence)

    local function handleWeaponSmithJobChangeQuest()

        local width = 400
        local height = 250
        local questId = 202
        local quest = player:findQuest(questId)

        if quest == nil then

            return false

        end

        if quest:getState() ~= 0 then

            return false

        end

        local step = quest:getFlag(1)

        if step == 0 then

            if sequence == 0 then

                local talkBox = NPCTalkBox:new(width, height)
                talkBox:addString(47002)

                player:talk(npc, talkBox)

                return true

            elseif sequence == 1 then

                local questChange = QuestChange:new()
                questChange:setFlag(1, 10)
                questChange:setFlag(2, 1411) -- 초원 일개미 id

                player:changeQuest(questId, questChange)

            end

        elseif step == 10 then

            local talkBox = NPCTalkBox:new(width, height)
            talkBox:addString(47003)

            player:talk(npc, talkBox)

        elseif step == 20 then

            if sequence == 0 then

                local talkBox = NPCTalkBox:new(width, height)
                talkBox:addString(47004)

                player:talk(npc, talkBox)

                return true

            elseif sequence == 1 then

                local removeItemId = 5050029 -- 광석
                local gainItemId = 5050028 -- 바람의 광석

                player:removeInventoryItem(removeItemId, 1)

                if player:addItem(gainItemId, 1) then

                    local questChange = QuestChange:new()
                    questChange:setFlag(1, 30)

                    player:changeQuest(questId, questChange)

                    local talkBox = NPCTalkBox:new(width, height)
                    talkBox:addString(47005)

                    player:talk(npc, talkBox)

                else

                    local talkBox = NPCTalkBox:new(width, height)
                    talkBox:addString(47006)

                    player:talk(npc, talkBox)

                end


            end

        elseif step == 30 then

            local talkBox = NPCTalkBox:new(width, height)
            talkBox:addString(47005)

            player:talk(npc, talkBox)

        end

        player:disposeTalk()

        return true

    end

    if handleWeaponSmithJobChangeQuest() then

        return

    end

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
