-- Zone  : 201
-- Stage : 10014
-- Name  : 랏슈 (검투사 전직 담당)
-- Nes   : 100031

return function (system, npc, player, sequence)

    local width = 400
    local height = 250

    if not player:isFighter() then

        local talkBox = NPCTalkBox:new(width, height)
        talkBox:addString(1650) -- 저어, 길드를 잘못 찾아오신 것이 아니신가요?<BR>여기는 전사길드입니다만, 죄송하지만 직업이 전사계가 아니신걸요

        player:talk(npc, talkBox)
        player:disposeTalk()

        return

    end

    if not player:isNovice() then

        local talkBox = NPCTalkBox:new(width, height)
        talkBox:addString(1660) -- 전사의 길로 착실하게 나아가고 계시네요? 앞으로도 열심히 해 주세요~

        player:talk(npc, talkBox)
        player:disposeTalk()

        return

    end

    local progressCheckQuestId = 1000
    local progressCheckQuest = player:findQuest(progressCheckQuestId)

    if progressCheckQuest == nil then

        if sequence == 0 then

            local talkBox = NPCTalkBox:new(width, height)
            talkBox:addString(1670) -- 모쪼록 저를 찾으실 일이 있으시면 부담없이 찾아주세요

            player:talk(npc, talkBox)

        elseif sequence == 1 then

            local talkBox = NPCTalkBox:new(width, height)
            talkBox:addString(1671) -- 저는 전직시험의 진행 체크만을 하고 있답니다. 다음 단계의 직업으로 전직할 자격이 생기면, 길드장 님께 찾아가 전직 자격시험에 응시하겠다고 말씀하도록 하고 자격을 얻도록 하세요

            player:talk(npc, talkBox)
            player:disposeTalk()

        end

        return

    end

    local targetJobId = 2100
    local jobId = progressCheckQuest:getFlag(0)

    if jobId ~= targetJobId then

        local talkBox = NPCTalkBox:new(width, height)
        talkBox:addString(1610) -- 어라, 다른 직업의 시험을 보는 중이시군요?

        player:talk(npc, talkBox)
        player:disposeTalk()

        return

    end

    local jobChangeQuestId = 1001
    local progressState = progressCheckQuest:getFlag(1)

    if progressState == 1 then

        local jobChangeQuest = player:findQuest(jobChangeQuestId)

        if jobChangeQuest ~= nil then

            -- 이미 한번 한 적 있으면
            if jobChangeQuest:getFlag(0) == 1 then

                if sequence == 0 then

                    local talkBox = NPCTalkBox:new(width, height)
                    talkBox:addString(1622) -- 다시 오실 거라고 믿었습니다. 자, 그럼 전직 시험을 다시 시작해 볼까요? 벌써 잊진 않으셨겠죠?

                    player:talk(npc, talkBox)

                    return

                elseif sequence == 1 then

                    local questChange = QuestChange:new()
                    questChange:setState(0, 0)

                    questChange:setFlag(0, 0)
                    questChange:setFlag(1, 2)
                    questChange:setFlag(2, 11802)
                    questChange:setFlag(3, 10)
                    questChange:setFlag(4, 0)
                    questChange:setFlag(5, 5050001)
                    questChange:setFlag(6, 1000000)
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
                talkBox:addString(1620) -- 아하, 전직을 하러 오셨군요?

                player:talk(npc, talkBox)

                return

            elseif sequence == 1 then

                local talkBox = NPCTalkBox:new(width, height)
                talkBox:addString(1621) -- <#FF4200>기드<#000000>씨로부터 들어온 부탁이 있군요. 가서 문제를 해결해주시고, 제게 돌아오시면 됩니다.

                player:talk(npc, talkBox)

                return

            elseif sequence == 2 then

                local quest = Quest:new(jobChangeQuestId)
                quest:setFlag(0, jobId)
                quest:setFlag(0, 0)
                quest:setFlag(1, 2)
                quest:setFlag(2, 11802) -- 잡아야 할 몬스터
                quest:setFlag(3, 10) -- 잡아야 할 마리수
                quest:setFlag(4, 0)
                quest:setFlag(5, 5050001) -- 드롭 아이템
                quest:setFlag(6, 1000000) -- 드롭 확률 (1/1000000 단위) << 100% 를 의도한듯??
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
            talkBox:addString(1640) -- 맡은 일은 잘 끝내신 것 같군요. 전직하실 충분한 자격이 생겼다고 길드장이신 레니 씨에게 전해두겠습니다

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
                talkBox:addString(1630) -- 아직 시험 과정이 끝나지 않으셨나보군요.
                talkBox:addMenu(1631, 1632, 1) -- 전직시험을 계속 진행한다
                talkBox:addMenu(1633, 1634, 2) -- 전직시험을 취소한다

                player:talk(npc, talkBox)

                return

            elseif sequence == 1 then

                if player:getSelection() == 2 then

                    local talkBox = NPCTalkBox:new(width, height)
                    talkBox:addString(1635) -- 어라? 도중에 취소하시는 건가요?

                    player:talk(npc, talkBox)

                    -- TODO: remove quest item

                    local jobChangeQuestChange = QuestChange:new()
                    jobChangeQuestChange:setFlag(0, 1)
                    jobChangeQuestChange:setFlag(1, 0)

                    player:changeQuest(jobChangeQuestId, jobChangeQuestChange)

                    local progressQuestChange = QuestChange:new()
                    progressQuestChange:setFlag(1, 0)
                    progressQuestChange:setFlag(0, 0)

                    player:changeQuest(progressCheckQuestId, progressQuestChange)

                end

            end

        end

    elseif progressState == 3 then -- 이미 보고 하고 또 말 건 경우

        local talkBox = NPCTalkBox:new(width, height)
        talkBox:addString(1640) -- 맡은 일은 잘 끝내신 것 같군요. 전직하실 충분한 자격이 생겼다고 길드장이신 레니 씨에게 전해두겠습니다

        player:talk(npc, talkBox)

    end

    player:disposeTalk()

end
