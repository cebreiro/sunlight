return function (system, npc, player, sequence)

    local width = 400
    local height = 250
    local stringBase = 6000

    local function handleJobChangeQuest(quest)

        local questId = quest:getId()
        local requiredItemId = quest:getFlag(5)
        local step = quest:getFlag(1)

        if step == 2 then -- 전직 퀘스트를 받고 처음

            if sequence == 0 then

                local talkBox = NPCTalkBox:new(width, height)
                talkBox:addString(stringBase + 10) -- 앗, 길드에서 오신 분이군요? 다행이다아~
                talkBox:addString(stringBase + 11) -- <#FF4200>무기점<#000000>의 <#FF4200>칼빈슨<#000000>씨에게 가서 자세한 얘기를 듣고 부디 도와주세요!

                player:talk(npc, talkBox)

                return

            elseif sequence == 1 then

                local questChange = QuestChange:new()
                questChange:setFlag(1, 3)

                player:changeQuest(questId, questChange)

            end

        elseif step == 3 then

            local talkBox = NPCTalkBox:new(width, height)
            talkBox:addString(stringBase + 12) -- 미소녀를 감히 습격하는 나아아쁜 놈들의 정체는 찾아내셨나요? 뭔가 단서가 될 만한 것을 발견하셨다면 꼭 가져와 주세요!

            player:talk(npc, talkBox)


        elseif step >= 4 and step <= 996 then

            if player:hasInventoryItem(requiredItemId, 1) then

                if sequence == 0 then

                    local talkBox = NPCTalkBox:new(width, height)
                    talkBox:addString(stringBase + 13) -- 앗, 돌아오셨군요? 그래, 의뢰는 무사히 해결해주셨나요? 미소녀의 안위는 어떻게 되었나요?
                    talkBox:addString(stringBase + 14) -- 이건 <#08BB00>8영웅<#000000> 으로 유명한 <#08BB00>윈 포담<#000000>의 문장이에요.

                    player:talk(npc, talkBox)

                    return

                elseif sequence == 1 then

                    player:removeInventoryItemAll(requiredItemId)

                    local questChange = QuestChange:new()
                    questChange:setFlag(1, 997)
                    questChange:setState(1)

                    player:changeQuest(questId, questChange)

                end

            else

                local talkBox = NPCTalkBox:new(width, height)
                talkBox:addString(stringBase + 12) -- 미소녀를 감히 습격하는 나아아쁜 놈들의 정체는 찾아내셨나요? 뭔가 단서가 될 만한 것을 발견하셨다면 꼭 가져와 주세요!

                player:talk(npc, talkBox)

            end


        elseif step == 997 then

            local talkBox = NPCTalkBox:new(width, height)
            talkBox:addString(stringBase + 15) -- 어, 왜 다시 오셨어요?

            player:talk(npc, talkBox)

        end

        player:disposeTalk()

    end

    local list = {}
    table.insert(list, player:findQuest(1002))
    table.insert(list, player:findQuest(1003))

    for i, jobChangeQuest in ipairs(list) do

        local step = jobChangeQuest:getFlag(1)

        if step >= 2 and step ~= 999 then

            handleJobChangeQuest(jobChangeQuest)

            return

        end

    end

    local chefQuestId = 201
    local chefQuest = player:findQuest(201)

    if chefQuest ~= nil then

        local state = chefQuest:getState()
        if state == 0 then

            local step = chefQuest:getFlag(1)

            if step == 10 then

                local subStep = chefQuest:getFlag(2)

                if subStep == 0 then

                    if sequence == 0 then

                        local talkBox = NPCTalkBox:new(width, height)
                        talkBox:addString(46002) -- 네넷? 칼빈슨씨의 단검이요? <#FF0000>죽순 팬더<#000000>였을 거에요

                        player:talk(npc, talkBox)

                        return

                    elseif sequence == 1 then

                        local questChange = QuestChange:new()
                        questChange:setFlag(2, 1)
                        questChange:setFlag(5, 701) -- 죽순 팬더

                        player:changeQuest(chefQuestId, questChange)

                    end

                elseif subStep == 1 then

                    local talkBox = NPCTalkBox:new(width, height)
                    talkBox:addString(46004) -- 단검을 찾던 못 찾든 간에… 꼭 칼빈슨 씨에게 전해주세요. 제가 너무나 죄송해 하고 있다구요. 알았죠?

                    player:talk(npc, talkBox)

                end

                player:disposeTalk()
                return

            elseif step == 20 then

                local talkBox = NPCTalkBox:new(width, height)
                talkBox:addString(46003) -- 네? 단검을 찾았다구요? 휴~~ 정말 다행이군요.

                player:talk(npc, talkBox)

                player:disposeTalk()
                return

            end

        end

    end

    local chemistQuestId = 204
    local chemistQuest = player:findQuest(204)

    if chemistQuest ~= nil then

        if chemistQuest:getState() == 0 then

            local step = chemistQuest:getFlag(3)

            if step == 0 then

                if sequence == 0 then

                    local talkBox = NPCTalkBox:new(width, height)
                    talkBox:addString(46006)

                    player:talk(npc, talkBox)

                    return

                elseif sequence == 1 then

                    local questChange = QuestChange:new()
                    questChange:setFlag(3, 1)

                    player:changeQuest(chemistQuestId, questChange)

                end

            elseif step == 1 then

                local talkBox = NPCTalkBox:new(width, height)
                talkBox:addString(46008)

                player:talk(npc, talkBox)

            elseif step == 2 then

                if sequence == 0 then

                    local talkBox = NPCTalkBox:new(width, height)
                    talkBox:addString(46007)

                    player:talk(npc, talkBox)

                    return

                elseif sequence == 1 then

                    player:removeInventoryItemAll(5050023) -- 반지

                    local questChange = QuestChange:new()
                    questChange:setFlag(1, 1)
                    questChange:setFlag(3, 3)

                    player:changeQuest(chemistQuestId, questChange)

                end

            elseif step == 3 then

                local talkBox = NPCTalkBox:new(width, height)
                talkBox:addString(46009)

                player:talk(npc, talkBox)

            end

            player:disposeTalk()

            return

        end

    end

    -- 서브잡이 있으면 스위치 메뉴
    local hasSubJob = false

    if sequence == 0 then

        local talkBox = NPCTalkBox:new(width, 200)
        talkBox:addString(stringBase + 1) -- 전 <#08BB00>판도라 네스트<#000000>의 민원을 담당하고 있는 카엘이에요.

        if hasSubJob then

            talkBox:addMenu(stringBase + 30, stringBase + 31, 1) -- 주직업과 부직업을 바꾼다.
            talkBox:addMenu(203, 204, 999) -- 대화를 끝낸다

        end

        player:talk(npc, talkBox)

        if hasSubJob then

            return

        end

    elseif sequence == 1 then

        if hasSubJob and player:getSelection() == 1 then

            -- TODO: implement here

        end


    end

    player:disposeTalk()

end
