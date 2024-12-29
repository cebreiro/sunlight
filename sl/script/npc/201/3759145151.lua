return function (system, npc, player, sequence)

    local width = 400
    local height = 250
    local stringBase = 27000
    local progressQuestId = 210
    local thisJobChangeSelection = 1
    local questId = 201
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

                if quest ~= nil then

                    if sequence == 0 then

                        local talkBox = NPCTalkBox:new(width, height)
                        talkBox:addString(stringBase + 16) -- 다시 요리사가 되고 싶어서 왔다고? 뭐 전직 시험 응시의 기회는 얼마든지 있으니 큰 상관은 없겠지

                        player:talk(npc, talkBox)

                        return

                    elseif sequence == 1 then

                        local questChange = QuestChange:new()
                        questChange:setState(0)
                        questChange:setFlag(0, 0)
                        questChange:setFlag(1, 0)
                        questChange:setFlag(2, 0)
                        questChange:setFlag(3, 0)
                        questChange:setFlag(4, 0)
                        questChange:setFlag(5, 0)

                        player:changeQuest(questId, questChange)

                        local progressQuestChange = QuestChange:new()
                        progressQuestChange:setFlag(1, 3)

                        player:changeQuest(progressQuestId, progressQuestChange)

                    end

                else

                    if sequence == 0 then

                        local talkBox = NPCTalkBox:new(width, height)
                        talkBox:addString(stringBase + 4) -- 요리사가 되고 싶어서 나를 찾아 왔다고? <BR>
                        talkBox:addString(stringBase + 5) --  벨슈로트 레스토랑에서 맛있는 음식 제작을 위해 항상 노력하고 계신 ‘메리’아주머니에게 멋진 음식을 선물
                        talkBox:addString(stringBase + 6) -- 자, 그럼 지금부터 <#FF4200>주먹밥, 칼빈슨의 바다라 고기, 밀가루<#000000> 3가지 재료를 모두 구해 오도록!

                        player:talk(npc, talkBox)

                        return

                    elseif sequence == 1 then

                        local quest = Quest:new(questId)
                        quest:setState(0)
                        quest:setFlag(0, 0)
                        quest:setFlag(1, 0)
                        quest:setFlag(2, 0)
                        quest:setFlag(3, 0)
                        quest:setFlag(4, 0)
                        quest:setFlag(5, 0)

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
                        talkBox:addString(stringBase + 20) -- 이봐, 아직 5분이 지나지 않았어. 너무 성급하게 굴면 요리를 망칠 수도 있다니까. 5분이 지난 후에 날 찾아오도록 해.

                        player:talk(npc, talkBox)

                    else

                        if sequence == 0 then

                            local talkBox = NPCTalkBox:new(width, height)
                            talkBox:addString(stringBase + 21) -- 좋아. 아주 멋진 ‘바다라 돈가스’가 완성이 되었네.

                            player:talk(npc, talkBox)

                            return

                        elseif sequence == 1 then

                            -- 돈까스
                            local itemId = 5050025

                            if player:addItem(itemId, 1) then

                                local questChange = QuestChange:new()
                                questChange:setFlag(0, 20)

                                player:changeQuest(questId, questChange)

                            else

                                local talkBox = NPCTalkBox:new(width, height)
                                talkBox:addString(stringBase + 22) -- 인벤이 꽉차서 줄수가 없군. 비운뒤 다시 오게

                                player:talk(npc, talkBox)

                            end

                        end

                    end

                elseif phase == 20 then

                    local talkBox = NPCTalkBox:new(width, height)
                    talkBox:addString(stringBase + 23) -- 아직 돈가스를 메리 아주머니에게 전해주지 않았군.

                    player:talk(npc, talkBox)

                elseif phase == 30 then

                    if sequence == 0 then

                        local talkBox = NPCTalkBox:new(width, height)
                        talkBox:addString(stringBase + 24) -- 그래 어떤가? 메리 아주머니께서 아주 좋아하셨다고?

                        player:talk(npc, talkBox)

                        return

                    elseif sequence == 1 then

                        local questChange = QuestChange:new()
                        questChange:setFlag(0, 40)

                        player:changeQuest(questId, questChange)

                        local progressQuestChange = QuestChange:new()
                        progressQuestChange:setFlag(1, 5)

                        player:changeQuest(progressQuestId, progressQuestChange)

                    end


                elseif phase == 40 then

                    local talkBox = NPCTalkBox:new(width, height)
                    talkBox:addString(stringBase + 25) -- 어서 길드장님을 만나보게

                    player:talk(npc, talkBox)

                else

                    local hasItem1 = false;

                    -- 바다라 고기
                    local itemId1 = 5050024

                    if player:hasInventoryItem(itemId1, 1) then

                        hasItem1 = true

                    end

                    -- 주먹밥
                    local hasItem2 = false;
                    local itemList1 = { 14030050, 24030050, 34030050 }
                    local itemList1Index = -1

                    if hasItem1 then

                        for k, v in pairs(itemList1) do

                            if player:hasInventoryItem(v, 1) then

                                hasItem2 = true
                                itemList1Index = k

                                break

                            end

                        end

                    end

                    -- 밀가루
                    local hasItem3 = false;
                    local itemList2 = { 17030370, 27030370, 37030370 }
                    local itemList2Index = -1

                    if hasItem2 then

                        for k, v in pairs(itemList2) do

                            if player:hasInventoryItem(v, 1) then

                                hasItem3 = true
                                itemList2Index = k

                                break

                            end

                        end

                    end

                    local hasItems = hasItem1 and hasItem2 and hasItem3

                    if hasItems then

                        if sequence == 0 then

                            local talkBox = NPCTalkBox:new(width, height)
                            talkBox:addString(stringBase + 18) -- 재료를 모두 구해왔군. 지금부터 <#FF4200>5분 정도<#000000>의 시간이면 충분하네

                            player:talk(npc, talkBox)

                            return

                        elseif sequence == 1 then

                            local questChange = QuestChange:new()
                            questChange:setFlag(4, system:getSeconds())
                            questChange:setFlag(0, 10)

                            player:changeQuest(questId, questChange)

                            player:removeInventoryItem(itemId1, 1)
                            player:removeInventoryItem(itemList1[itemList1Index], 1)
                            player:removeInventoryItem(itemList2[itemList2Index], 1)

                        end

                    else

                        if sequence == 0 then

                            local talkBox = NPCTalkBox:new(width, height)
                            talkBox:addString(stringBase + 8) -- 메리 아주머니에게 선물할 돈가스의 재료를 구해오는 일은 잘 되어 가고 있나? 바다라 고기는 꼭 ‘칼빈슨의 바다라 고기’를 가져오라고 한 것도 기억하고 있겠지?

                            player:talk(npc, talkBox)

                            return

                        elseif sequence == 1 then

                            local talkBox = NPCTalkBox:new(width, height)
                            talkBox:addString(stringBase + 9) -- 만약 시험이 자네에게 너무 힘들다고 생각이 되면 언제든지 이야기 하도록 해. 내가 시험을 취소해 줄 수 있으니까.
                            talkBox:addMenu(stringBase + 10, stringBase + 11, 1) -- 전직 시험을 계속 진행한다
                            talkBox:addMenu(stringBase + 12, stringBase + 13, 2) -- 전직 시험을 취소한다

                            player:talk(npc, talkBox)

                            return


                        elseif sequence == 2 then

                            if player:getSelection() == 2 then

                                player:removeInventoryItemAll(5050019) -- 칼빈슨의 단검
                                player:removeInventoryItemAll(5050024) -- 바다라 고기

                                local talkBox = NPCTalkBox:new(width, height)
                                talkBox:addString(stringBase + 15) -- 아직 요리사가 될 마음의 준비가 되어있지 않은 것 같군.

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
                talkBox:addString(stringBase + 26) -- 축하하네. 이제는 당당히 한 사람 몫을 할 요리사가 되었군

                player:talk(npc, talkBox)
                player:disposeTalk()

                return


            elseif step == 5 then

                local talkBox = NPCTalkBox:new(width, height)
                talkBox:addString(stringBase + 25) -- 어서 길드장님을 만나보게

                player:talk(npc, talkBox)

                player:disposeTalk()

                return


            end

        else

            if step == 3 then

                local talkBox = NPCTalkBox:new(width, height)
                talkBox:addString(stringBase + 17) -- 다른 전직 시험을 보는 동안에는 요리사 전직 시험을 볼 수 없으니

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
                talkBox:addString(stringBase + 27) -- 흐음~ 전직을 위해 이곳을 들렀었던 조합사가 아닌가? 전직하고 나선 더욱 멋진 모습으로 바뀌었구만

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
