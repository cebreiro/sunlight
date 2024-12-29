return function (system, npc, player, sequence)

    local width = 400
    local height = 250
    local stringBase = 29000
    local progressQuestId = 210
    local thisJobChangeSelection = 3
    local questId = 203
    local questList = { 201, 202, 203, 204 }

    -- 실타레
    local itemId1 = 5050027

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

                    local stringNumber = (quest ~= nil) and stringBase + 14 or stringBase + 4

                    local talkBox = NPCTalkBox:new(width, height)
                    talkBox:addString(stringNumber)

                    player:talk(npc, talkBox)

                    return

                elseif sequence == 1 then

                    local failToAddItem = false

                    for i = 1, 3 do

                        if not player:addItem(itemId1, 1) then

                            failToAddItem = true

                            break

                        end

                    end

                    if failToAddItem then

                        player:removeInventoryItemAll(itemId1)

                        local talkBox = NPCTalkBox:new(width, height)
                        talkBox:addString(stringBase + 18)

                        player:talk(npc, talkBox)
                        player:disposeTalk()

                        return

                    end

                    if quest ~= nil then

                        local questChange = QuestChange:new()
                        questChange:setState(0)
                        questChange:setFlag(0, 0)
                        questChange:setFlag(1, 0)
                        questChange:setFlag(2, 0)
                        questChange:setFlag(3, 0)
                        questChange:setFlag(4, 0)
                        questChange:setFlag(5, 0)
                        questChange:setFlag(6, 0)

                        player:changeQuest(questId, questChange)

                        local progressQuestChange = QuestChange:new()
                        progressQuestChange:setFlag(1, 3)

                        player:changeQuest(progressQuestId, progressQuestChange)

                    else

                        local quest = Quest:new(questId)
                        quest:setState(0)
                        quest:setFlag(0, 0)
                        quest:setFlag(1, 0)
                        quest:setFlag(2, 0)
                        quest:setFlag(3, 0)
                        quest:setFlag(4, 0)
                        quest:setFlag(5, 0)
                        quest:setFlag(6, 0)

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

                if phase == 40 then

                    local talkBox = NPCTalkBox:new(width, height)
                    talkBox:addString(stringBase + 17)

                    player:talk(npc, talkBox)
                    player:disposeTalk()

                    return

                end

                -- 모두 만나고 왔는가 체크
                local flag1 = quest:getFlag(1) ~= 0
                local flag2 = quest:getFlag(2) ~= 0
                local flag3 = quest:getFlag(3) ~= 0

                if flag1 and flag2 and flag3 then

                    if sequence == 0 then

                        local talkBox = NPCTalkBox:new(width, height)
                        talkBox:addString(stringBase + 15)

                        player:talk(npc, talkBox)

                        return

                    elseif sequence == 1 then

                        local questChange = QuestChange:new()
                        questChange:setState(0)
                        questChange:setFlag(0, 40)

                        player:changeQuest(questId, questChange)

                        local progressQuestChange = QuestChange:new()
                        progressQuestChange:setFlag(1, 5)

                        player:changeQuest(progressQuestId, progressQuestChange)

                    end

                    player:disposeTalk()

                    return

                end

                if sequence == 0 then

                    local talkBox = NPCTalkBox:new(width, height)
                    talkBox:addString(stringBase + 6)
                    talkBox:addMenu(stringBase + 8, stringBase + 9, 1)
                    talkBox:addMenu(stringBase + 10, stringBase + 11, 2)

                    player:talk(npc, talkBox)

                    return

                elseif sequence == 1 then

                    if player:getSelection() == 2 then

                        player:removeInventoryItemAll(itemId1)
                        player:removeInventoryItemAll(5050026) -- 헤벌레 실크

                        local talkBox = NPCTalkBox:new(width, height)
                        talkBox:addString(stringBase + 13)

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

                player:disposeTalk()

                return

            elseif step == 4 then

                local talkBox = NPCTalkBox:new(width, height)
                talkBox:addString(stringBase + 16)

                player:talk(npc, talkBox)
                player:disposeTalk()

                return

            elseif step == 5 then

                local talkBox = NPCTalkBox:new(width, height)
                talkBox:addString(stringBase + 17)

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
