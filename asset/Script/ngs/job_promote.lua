-- Description : 초보 전사 전직관 스크립트

function handleScript_JobChangeSupervisor (system, npc, player, sequence, param)

    local width = 400
    local height = 250
    local stringBase = param.stringBase
    local jobChangeQuestId = param.questId
    local progressCheckQuestId = 1000
    local targetJobId = param.jobId
    local requiredItemId = param.requiredItemId
    local monsterId = param.monsterId
    local monsterCount = param.monsterCount
    local probability = param.probability

    if not player:isFighter() then

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
                    questChange:configureItemGain(monsterId, requiredItemId, probability, 1, monsterCount)

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
                quest:configureItemGain(monsterId, requiredItemId, probability, 1, monsterCount)

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
                jobChangeQuestChange:resetItemGain()

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
    handleScript_JobChangeSupervisor = handleScript_JobChangeSupervisor
}
