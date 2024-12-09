-- Description : 전직관 스크립트

function isFighter (player)
    return player:isFighter()
end

function isRanger (player)
    return player:isRanger()
end

function isMagician (player)
    return player:isMagician()
end

function isArtisan (player)
    return player:isArtisan()
end

function handleScript_JobChangGuildMaster (system, npc, player, sequence, param)
    local width = 400
    local height = 250

    local progressCheckQuestId = param.progressCheckQuestId
    local jobChangeQuestTable = param.jobChangeQuestTable
    local stringBase = param.stringBase

    if not param.isValidJob(player) then

        local talkBox = NPCTalkBox:new(width, height)
        talkBox:addString(stringBase + 10)

        player:talk(npc, talkBox)
        player:disposeTalk()

        return

    end

    if not player:isNovice() then

        local talkBox = NPCTalkBox:new(width, height)
        talkBox:addString(stringBase + 20)

        player:talk(npc, talkBox)
        player:disposeTalk()

        return

    end

    local progressCheckQuest = player:findQuest(progressCheckQuestId)

    if progressCheckQuest ~= nil then

        local step = progressCheckQuest:getFlag(1)

        if step == 2 then -- 전직 담당에게서 퀘스트 받은 경우

            local talkBox = NPCTalkBox:new(width, height)
            talkBox:addString(stringBase + 120)

            player:talk(npc, talkBox)
            player:disposeTalk()

            return

        elseif step == 3 then -- 성공 후 전직 담당과 얘기 끝

            if sequence == 0 then

                local talkBox = NPCTalkBox:new(width, height)
                talkBox:addString(stringBase + 130)

                player:talk(npc, talkBox)

                return

            elseif sequence == 1 then

                local talkBox = NPCTalkBox:new(width, height)
                talkBox:addString(stringBase + 131)

                player:talk(npc, talkBox)

                return

            elseif sequence == 2 then

                local advancedJob = progressCheckQuest:getFlag(2)

                if not player:promoteJob(advancedJob) then

                    system:error("fail to premote job change quest - job: " .. advancedJob)
                    player:disposeTalk()

                    return

                end

                local progressQuestChange = QuestChange:new()
                progressQuestChange:setFlag(0, 0)
                progressQuestChange:setFlag(1, 999)

                player:changeQuest(progressCheckQuestId, progressQuestChange)

                local jobChangeQuestId = progressCheckQuest:getFlag(0)
                local jobChangeQuest = player:findQuest(jobChangeQuestId)

                if jobChangeQuest == nil then

                    system:error("fail to find job change quest")

                else

                    local jobChangeQuestChange = QuestChange:new()
                    jobChangeQuestChange:setState(1)
                    jobChangeQuestChange:setFlag(1, 999)

                    player:changeQuest(jobChangeQuestId, jobChangeQuestChange)

                end

            end

            player:disposeTalk()

            return

        end

    end


    if sequence == 0 then

        local talkBox = NPCTalkBox:new(width, height)
        talkBox:addString(stringBase + 30)

        player:talk(npc, talkBox)

        return

    elseif sequence == 1 then

        local talkBox = NPCTalkBox:new(width, height)
        talkBox:addString(stringBase + 31)
        talkBox:addMenu(stringBase + 32, stringBase + 33, 1)
        talkBox:addMenu(stringBase + 34, stringBase + 35, 2)
        talkBox:addMenu(stringBase + 36, stringBase + 37, 3)
        talkBox:addMenu(stringBase + 38, stringBase + 39, 4)

        player:talk(npc, talkBox)

        return

    elseif sequence == 2 then

        local selection = player:getSelection()

        player:setState(selection)

        if selection == 1 then -- 직업설명

            local talkBox = NPCTalkBox:new(width, height)
            talkBox:addString(stringBase + 50)
            talkBox:addMenu(stringBase + 51, stringBase + 52, 1)
            talkBox:addMenu(stringBase + 53, stringBase + 54, 2)
            talkBox:addMenu(stringBase + 55, stringBase + 56, 3)
            talkBox:addMenu(stringBase + 57, stringBase + 58, 4)

            player:talk(npc, talkBox)

            return

        elseif selection == 2 then -- 전직설명

            local talkBox = NPCTalkBox:new(width, height)
            talkBox:addString(stringBase + 70)
            talkBox:addString(stringBase + 71)

            player:talk(npc, talkBox)
            player:setSequence(0) -- goto sequence 1

            return

        elseif selection == 3 then -- 전직신청

            if player:getNoviceJobLevel() < 20 then

                local talkBox = NPCTalkBox:new(width, height)
                talkBox:addString(stringBase + 80)

                player:talk(npc, talkBox)
                player:setSequence(0) -- goto sequence 1

                return

            else

                local talkBox = NPCTalkBox:new(width, height)
                talkBox:addString(stringBase + 90)
                talkBox:addMenu(stringBase + 91, stringBase + 92, 1)
                talkBox:addMenu(stringBase + 93, stringBase + 94, 2)
                talkBox:addMenu(stringBase + 95, stringBase + 96, 3)
                talkBox:addMenu(stringBase + 97, stringBase + 98, 4)
                talkBox:addMenu(stringBase + 99, stringBase + 100, 999)

                player:talk(npc, talkBox)

                return

            end

        elseif selection == 4 then -- 대화 끝내기

            local talkBox = NPCTalkBox:new(width, height)
            talkBox:addString(stringBase + 110)

            player:talk(npc, talkBox)

            return

        end

    elseif sequence == 3 then

        local prevSelection = player:getState()
        local selection = player:getSelection()

        if prevSelection == 1 then -- 직업 설명

            local talkBox = NPCTalkBox:new(width, height)
            talkBox:addString(stringBase + 58 + selection)

            player:talk(npc, talkBox)
            player:setSequence(0) -- goto sequence 1

            return

        elseif prevSelection == 3 then -- 전직신청

            if selection == 999 then

                -- 나중에 신청하겠다는 경우
                local talkBox = NPCTalkBox:new(width, height)
                talkBox:addString(stringBase + 101)

                player:talk(npc, talkBox)
                player:disposeTalk()

                return

            end

            local jobChangeQuestId = jobChangeQuestTable[selection] or -1

            if progressCheckQuest ~= nil then

                local questChange = QuestChange:new()
                questChange:setState(3)
                questChange:setFlag(0, jobChangeQuestId)
                questChange:setFlag(1, 1)

                player:changeQuest(progressCheckQuestId, questChange)

            else

                local quest = Quest:new(progressCheckQuestId)
                quest:setState(3)
                quest:setFlag(0, jobChangeQuestId)
                quest:setFlag(1, 1)

                player:startQuest(quest)
            end

        end

    end

    player:disposeTalk()

end

function handleScript_JobChangeSupervisor (system, npc, player, sequence, param)

    local width = 400
    local height = 250
    local stringBase = param.stringBase
    local jobChangeQuestId = param.questId
    local progressCheckQuestId = param.progressCheckQuestId
    local targetJobId = param.jobId
    local requiredItemId = param.requiredItemId
    local monsterId = param.monsterId
    local monsterCount = param.monsterCount
    local probability = param.probability

    if not param.isValidJob(player) then

        local talkBox = NPCTalkBox:new(width, height)
        talkBox:addString(stringBase + 50)

        player:talk(npc, talkBox)
        player:disposeTalk()

        return

    end

    if not player:isNovice() then

        local talkBox = NPCTalkBox:new(width, height)
        talkBox:addString(stringBase + 60)

        player:talk(npc, talkBox)
        player:disposeTalk()

        return

    end

    local progressCheckQuest = player:findQuest(progressCheckQuestId)

    if progressCheckQuest == nil then

        if sequence == 0 then

            local talkBox = NPCTalkBox:new(width, height)
            talkBox:addString(stringBase + 70)

            player:talk(npc, talkBox)

        elseif sequence == 1 then

            local talkBox = NPCTalkBox:new(width, height)
            talkBox:addString(stringBase + 71)

            player:talk(npc, talkBox)
            player:disposeTalk()

        end

        return

    end

    if progressCheckQuest:getFlag(0) ~= jobChangeQuestId then

        local talkBox = NPCTalkBox:new(width, height)
        talkBox:addString(stringBase + 10)

        player:talk(npc, talkBox)
        player:disposeTalk()

        return

    end

    local progressState = progressCheckQuest:getFlag(1)

    if progressState == 1 then

        local jobChangeQuest = player:findQuest(jobChangeQuestId)

        if jobChangeQuest ~= nil then

            -- 이미 한번 한 적 있으면
            if jobChangeQuest:getFlag(0) == 1 then

                if sequence == 0 then

                    local talkBox = NPCTalkBox:new(width, height)
                    talkBox:addString(stringBase + 22)

                    player:talk(npc, talkBox)

                    return

                elseif sequence == 1 then

                    local questChange = QuestChange:new()
                    questChange:setState(0, 0)

                    questChange:setFlag(0, 0)
                    questChange:setFlag(1, 2)
                    questChange:setFlag(2, monsterId)
                    questChange:setFlag(3, monsterCount)
                    questChange:setFlag(4, 0)
                    questChange:setFlag(5, requiredItemId)
                    questChange:setFlag(6, probability)
                    questChange:setFlag(7, 0)
                    questChange:setFlag(8, 0)
                    questChange:setFlag(9, 0)

                    player:changeQuest(jobChangeQuestId, questChange)

                    local progressQuestChange = QuestChange:new()
                    progressQuestChange:setFlag(1, 2) -- goto state == 2
                    progressQuestChange:setFlag(2, targetJobId)

                    player:changeQuest(progressCheckQuestId, progressQuestChange)

                end

            end

        else

            if sequence == 0 then

                local talkBox = NPCTalkBox:new(width, height)
                talkBox:addString(stringBase + 20)

                player:talk(npc, talkBox)

                return

            elseif sequence == 1 then

                local talkBox = NPCTalkBox:new(width, height)
                talkBox:addString(stringBase + 21)

                player:talk(npc, talkBox)

                return

            elseif sequence == 2 then

                local quest = Quest:new(jobChangeQuestId)

                quest:setFlag(0, 0)
                quest:setFlag(1, 2)
                quest:setFlag(2, monsterId)
                quest:setFlag(3, monsterCount)
                quest:setFlag(4, 0)
                quest:setFlag(5, requiredItemId)
                quest:setFlag(6, probability)
                quest:setFlag(7, 0)
                quest:setFlag(8, 0)
                quest:setFlag(9, 0)

                player:startQuest(quest)

                local progressQuestChange = QuestChange:new()
                progressQuestChange:setFlag(1, 2) -- goto progressState == 2
                progressQuestChange:setFlag(2, targetJobId)

                player:changeQuest(progressCheckQuestId, progressQuestChange)

            end
                
        end

    elseif progressState == 2 then

        local jobChangeQuest = player:findQuest(jobChangeQuestId)
        if jobChangeQuest == nil then

            system:error("job change quest is null")

            player:disposeTalk()
            return

        end

        local jobChangeQuestState = jobChangeQuest:getState()

        if jobChangeQuestState == 1 then

            local talkBox = NPCTalkBox:new(width, height)
            talkBox:addString(stringBase + 40)

            player:talk(npc, talkBox)

            local jobChangeQuestChange = QuestChange:new()
            jobChangeQuestChange:setFlag(1, 998)

            player:changeQuest(jobChangeQuestId, jobChangeQuestChange)

            local progressQuestChange = QuestChange:new()
            progressQuestChange:setFlag(1, 3)

            player:changeQuest(progressCheckQuestId, progressQuestChange)

        else

            if sequence == 0 then

                local talkBox = NPCTalkBox:new(width, 200)
                talkBox:addString(stringBase + 30)
                talkBox:addMenu(stringBase + 31, stringBase + 32, 1)
                talkBox:addMenu(stringBase + 33, stringBase + 34, 2)

                player:talk(npc, talkBox)

                return

            elseif sequence == 1 then

                if player:getSelection() == 2 then

                    local talkBox = NPCTalkBox:new(width, height)
                    talkBox:addString(stringBase + 35)

                    player:talk(npc, talkBox)

                    return

                end

            elseif sequence == 2 then

                player:removeInventoryItemAll(requiredItemId)

                local jobChangeQuestChange = QuestChange:new()
                jobChangeQuestChange:setState(2)
                jobChangeQuestChange:setFlag(0, 1)
                jobChangeQuestChange:setFlag(1, 0)

                player:changeQuest(jobChangeQuestId, jobChangeQuestChange)

                local progressQuestChange = QuestChange:new()
                progressQuestChange:setFlag(1, 0)
                progressQuestChange:setFlag(0, 0)

                player:changeQuest(progressCheckQuestId, progressQuestChange)

            end

        end

    elseif progressState == 3 then -- 이미 보고 하고 또 말 건 경우

        local talkBox = NPCTalkBox:new(width, height)
        talkBox:addString(stringBase + 40)

        player:talk(npc, talkBox)

    end

    player:disposeTalk()

end

return {
    isFighter = isFighter,
    isRanger = isRanger,
    isMagician = isMagician,
    isArtisan, isArtisan,
    handleScript_JobChangGuildMaster = handleScript_JobChangGuildMaster,
    handleScript_JobChangeSupervisor = handleScript_JobChangeSupervisor
}
