return function (system, npc, player, sequence)

    local width = 400
    local height = 250
    local stringBase = 10300

    local list = {}
    table.insert(list, player:findQuest(1002))
    table.insert(list, player:findQuest(1003))

    for i, quest in ipairs(list) do

        local step = quest:getFlag(1)

        if step > 2 and step < 5 then

            local talkBox = NPCTalkBox:new(width, height)
            talkBox:addString(stringBase + 10) -- <#FF4200>보라색 머리의 소녀<#000000>가 습격당하는 장면 말이지?
            talkBox:addString(stringBase + 11) -- 실은 이미 그 소녀가 무시무시한 파워로 녀석들에게 본때를 보여준 뒤였어. 나 같은 건 나설 곳도 없었다구.
            talkBox:addString(stringBase + 12) -- <#FF4200>버려진 연구소<#000000>에 가서 녀석들을 퇴치하면 복수는 이루어지는 게 아닐까?

            player:talk(npc, talkBox)

            if step ~= 4 then

                local questChange = QuestChange:new()
                questChange:setFlag(1, 4)

                player:changeQuest(quest:getId(), questChange)

            end

            player:disposeTalk()

            return

        end

    end

    local quest = player:findQuest(1103)
    if quest ~= nil then

        local step = quest:getFlag(1)

        if step == 3 then

            if sequence == 0 then

                local talkBox = NPCTalkBox:new(width, height)
                talkBox:addString(stringBase + 20)
                talkBox:addString(stringBase + 21)
                talkBox:addString(stringBase + 22)

                player:talk(npc, talkBox)

                return

            elseif sequence == 1 then

                local questChange = QuestChange:new()
                questChange:setFlag(1, 4)

                player:changeQuest(quest:getId(), questChange)

            end

            player:disposeTalk()

            return

        elseif step == 4 then

            local talkBox = NPCTalkBox:new(width, height)
            talkBox:addString(stringBase + 23)

            player:talk(npc, talkBox)
            player:disposeTalk()

            return

        end

    end

    local chefQuestId = 201
    local chefQuest = player:findQuest(201)

    if chefQuest ~= nil then

        local state = chefQuest:getState()
        if state == 0 then

            local step = chefQuest:getFlag(1)

            if step == 0 then

                if sequence == 0 then

                    local talkBox = NPCTalkBox:new(width, height)
                    talkBox:addString(40126) -- 음… 바다라의 고기가 필요하다고?<BR> 

                    player:talk(npc, talkBox)

                    return

                elseif sequence == 1 then

                    local questChange = QuestChange:new()
                    questChange:setFlag(1, 10)

                    player:changeQuest(chefQuestId, questChange)

                end

                player:disposeTalk()

                return

            elseif step == 10 then

                local talkBox = NPCTalkBox:new(width, height)
                talkBox:addString(40127) -- 먼저 카엘을 한번 만나보게나. 그 녀석이 단검을 잃어버린 장소에서 찾는다면 쉽게 찾을 수도 있을 테니까.

                player:talk(npc, talkBox)
                player:disposeTalk()

                return

            elseif step == 20 then

                if sequence == 0 then

                    local talkBox = NPCTalkBox:new(width, height)
                    talkBox:addString(40128) -- 자네 정말 단검을 찾아가지고 왔군!

                    player:talk(npc, talkBox)

                    return

                elseif sequence == 1 then

                    local gainItemId = 5050024
                    local removeItemId = 5050019

                    if player:addItem(gainItemId, 1) then

                        player:removeInventoryItemAll(removeItemId)

                        local questChange = QuestChange:new()
                        questChange:setFlag(1, 30)

                        player:changeQuest(chefQuestId, questChange)

                    else

                        local talkBox = NPCTalkBox:new(width, height)
                        talkBox:addString(40117) -- 인벤이 꽉차서 줄수가 없네. 비운뒤 다시오게

                        player:talk(npc, talkBox)

                    end

                end

                player:disposeTalk()

                return

            elseif step == 30 then

                local talkBox = NPCTalkBox:new(width, height)
                talkBox:addString(41127) -- 자네 덕에 단검을 되찾게 되어 정말 다행이네. 카엘 녀석도 이번 일을 계기로 앞으로 남의 물건에 각별히 신경 쓰게 될 걸세. 

                player:talk(npc, talkBox)
                player:disposeTalk()

                return

            end

        end

    end


    -- TODO: 녹슨 단검 퀘스트

    local talkBox = NPCTalkBox:new(width, height)
    talkBox:addString(stringBase + 1) -- 여어, 어서 오시게나. 모험가로구만?
    talkBox:addString(stringBase + 2)

    player:talk(npc, talkBox)

    player:disposeTalk()

end
