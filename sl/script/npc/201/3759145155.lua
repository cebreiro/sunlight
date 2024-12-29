return function (system, npc, player, sequence)

    local width = 400
    local height = 250
    local stringBase = 28000
    local progressQuestId = 210
    local thisJobChangeSelection = 2
    local questId = 202
    local questList = { 201, 202, 203, 204 }

    if not player:isArtisan() then

        local talkBox = NPCTalkBox:new(width, height)
        talkBox:addString(stringBase + 1)
        talkBox:addString(stringBase + 2)

        player:talk(npc, talkBox)
        player:disposeTalk()

    end


    local progressQuest = player:findQuest(progressQuestId)
    if progressQuest ~= nil then

        local selection = progressQuest:getFlag(0)
        local step = progressQuest:getFlag(1)

        if selection == thisJobChangeSelection then

            local quest = player:findQuest(questId)

            if step == 1 then

                if sequence == 0 then

                    local stringNumber = (quest ~= nil) and stringBase + 13 or stringBase + 4

                    local talkBox = NPCTalkBox:new(width, height)
                    talkBox:addString(stringNumber)

                    player:talk(npc, talkBox)

                    return

                elseif sequence == 1 then

                    if quest ~= nil then

                        local questChange = QuestChange:new()
                        questChange:setState(0)
                        questChange:setFlag(0, 0)
                        questChange:setFlag(1, 0)
                        questChange:setFlag(2, 0)
                        questChange:setFlag(3, 0)
                        questChange:setFlag(4, 6501)
                        questChange:setFlag(5, 0)

                        player:changeQuest(questId, questChange)

                        local progressQuestChange = QuestChange:new()
                        progressQuestChange:setFlag(1, 3)

                        player:changeQuest(progressQuestId, progressQuestChange)

                    else

                        local quest = Quest:new(questId)
                        quest:setState(0)
                        quest:setFlag(0, 0)
                        quest:setFlag(1, 0)
                        quest:setFlag(2, 0) -- 디안나 -> 초원 개미 id 세팅
                        quest:setFlag(3, 0) -- 초원 개미 kill count
                        quest:setFlag(4, 6501) -- 뿅망치 선생 id
                        quest:setFlag(5, 0) -- 뿅망치 kill count

                        player:startQuest(quest)

                        local progressQuestChange = QuestChange:new()
                        progressQuestChange:setFlag(1, 3)

                        player:changeQuest(progressQuestId, progressQuestChange)

                    end

                end

                player:disposeTalk()

                return

            elseif step == 3 then

                local phase = quest:getFlag(0)

                if phase == 10 then

                    local now = system:getSeconds()
                    local start = quest:getFlag(4)

                    local elapsed = now - start

                    if elapsed < 300 then

                        local talkBox = NPCTalkBox:new(width, height)
                        talkBox:addString(stringBase + 16)

                        player:talk(npc, talkBox)

                    else

                        if sequence == 0 then

                            local talkBox = NPCTalkBox:new(width, height)
                            talkBox:addString(stringBase + 17)

                            player:talk(npc, talkBox)

                            return

                        elseif sequence == 1 then

                            -- 윈드엣지
                            local itemId = 5050020

                            if player:addItem(itemId, 1) then

                                local questChange = QuestChange:new()
                                questChange:setFlag(0, 20)

                                player:changeQuest(questId, questChange)

                            else

                                local talkBox = NPCTalkBox:new(width, height)
                                talkBox:addString(stringBase + 22)

                                player:talk(npc, talkBox)

                            end

                        end

                    end

                elseif phase == 20 then

                    local talkBox = NPCTalkBox:new(width, height)
                    talkBox:addString(stringBase + 18)

                    player:talk(npc, talkBox)

                elseif phase == 30 then

                    if sequence == 0 then

                        local talkBox = NPCTalkBox:new(width, height)
                        talkBox:addString(stringBase + 19)

                        player:talk(npc, talkBox)

                        return

                    elseif sequence == 1 then

                        local quest = Quest:new(questId)
                        quest:setFlag(0, 40)

                        player:startQuest(quest)

                        local progressQuestChange = QuestChange:new()
                        progressQuestChange:setFlag(1, 5)

                        player:changeQuest(progressQuestId, progressQuestChange)

                    end

                elseif phase == 40 then

                    local talkBox = NPCTalkBox:new(width, height)
                    talkBox:addString(stringBase + 21)

                    player:talk(npc, talkBox)

                else

                    local itemId1 = 5050021 -- 그린 토파즈
                    local itemId2 = 5050028 -- 바람의 광석

                    if player:hasInventoryItem(itemId1, 1) and player:hasInventoryItem(itemId2, 1) then

                        if sequence == 0 then

                            local talkBox = NPCTalkBox:new(width, height)
                            talkBox:addString(stringBase + 15)

                            player:talk(npc, talkBox)

                            return

                        elseif sequence == 1 then

                            local questChange = QuestChange:new()
                            questChange:setFlag(4, system:getSeconds())
                            questChange:setFlag(0, 10)

                            player:changeQuest(questId, questChange)

                            player:removeInventoryItem(itemId1, 1)
                            player:removeInventoryItem(itemId2, 1)

                        end

                    else

                        if sequence == 0 then

                            local talkBox = NPCTalkBox:new(width, height)
                            talkBox:addString(stringBase + 6)
                            talkBox:addMenu(stringBase + 7, stringBase + 8, 1)
                            talkBox:addMenu(stringBase + 9, stringBase + 10, 2)

                            player:talk(npc, talkBox)

                            return

                        elseif sequence == 1 then

                            if player:getSelection() == 2 then

                                player:removeInventoryItem(itemId1, 1)
                                player:removeInventoryItem(itemId2, 1)

                                local talkBox = NPCTalkBox:new(width, height)
                                talkBox:addString(stringBase + 12)

                                player:talk(npc, talkBox)

                                local questChange = QuestChange:new()
                                questChange:setFlag(0, 0)
                                questChange:setFlag(1, 0)
                                questChange:setFlag(2, 0)
                                questChange:setFlag(3, 0)
                                questChange:setFlag(4, 0)
                                questChange:setFlag(5, 0)
                                questChange:setState(3)

                                player:changeQuest(questId, questChange)

                                local progressQuestChange = QuestChange:new()
                                progressQuestChange:setFlag(1, 2)

                                player:changeQuest(progressQuestId, progressQuestChange)

                            end

                        end

                    end

                end

                player:disposeTalk()

                return

            elseif step == 4 then

                local talkBox = NPCTalkBox:new(width, height)
                talkBox:addString(stringBase + 20)

                player:talk(npc, talkBox)
                player:disposeTalk()

                return

            elseif step == 5 then

                local talkBox = NPCTalkBox:new(width, height)
                talkBox:addString(stringBase + 21)

                player:talk(npc, talkBox)

                player:disposeTalk()

                return

            end

        else

            if step == 3 then

                local talkBox = NPCTalkBox:new(width, height)
                talkBox:addString(stringBase + 5)

                player:talk(npc, talkBox)
                player:disposeTalk()

                return

            end

        end

    end

    for k, v in pairs(questList) do

        if v ~= questId then

            local otherQuest = player:findQuest(v)
            if otherQuest ~= nil then

                local talkBox = NPCTalkBox:new(width, height)
                talkBox:addString(stringBase + 23)

                player:talk(npc, talkBox)
                player:disposeTalk()

                return

            end

        end

    end

    local talkBox = NPCTalkBox:new(width, height)
    talkBox:addString(stringBase + 3)

    player:talk(npc, talkBox)
    player:disposeTalk()

end
