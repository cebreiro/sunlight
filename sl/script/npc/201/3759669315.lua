return function (system, npc, player, sequence)

    local width = 400
    local height = 250

    local stringBase = 10300
    local questId = 1004
    local quest = player:findQuest(questId)

    local function processJobChange()

        local requiredItemId = quest:getFlag(5)
        local requiredMonsterCount = quest:getFlag(3)
        local step = quest:getFlag(1)

        if step == 2 then -- 전직 퀘스트를 받고 처음

            if sequence == 0 then

                local talkBox = NPCTalkBox:new(width, height)
                talkBox:addString(stringBase + 10) -- <#FF4200>사적스파이<#000000>들 때문에 말썽이 많았단 얘기는 들었지만서도... 막상 당하니까 남의 일이 아냐. 
                talkBox:addString(stringBase + 11) -- <#FF4200>무너진 마을<#000000>에 가서 <#FF4200>사적스파이<#000000>들을 좀 잡아 없애 주시게. 응?
                talkBox:addStringWithInt(stringBase + 12, requiredMonsterCount) --  <#FF4200>%d 명<#000000>을 잡아달라구!

                player:talk(npc, talkBox)

                return

            elseif sequence == 1 then

                local questChange = QuestChange:new()
                questChange:setFlag(1, 3)

                player:changeQuest(questId, questChange)

            end

        elseif step >= 3 and step <= 996 then

            if player:hasInventoryItem(requiredItemId, 1) then

                if sequence == 0 then

                    local talkBox = NPCTalkBox:new(width, height)
                    talkBox:addString(stringBase + 14) -- 어허, 의뢰를 완수했군.
                    talkBox:addString(stringBase + 15) -- 그럼 앞으로도 계속 수고하라구!!

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
                talkBox:addString(stringBase + 13) -- 아직 잡는 중인가? 거 퍽퍽 패서 팍팍 잡아오게나!

                player:talk(npc, talkBox)

            end

        elseif step == 997 then

            local talkBox = NPCTalkBox:new(width, height)
            talkBox:addString(stringBase + 16) -- 뭔가? 아직 볼 일이 남았남? 의뢰를 모두 끝냈다고 전직담당에게 말해두었네만...

            player:talk(npc, talkBox)

        end


        player:disposeTalk()

    end

    if quest ~= nil then

        local state = quest:getFlag(1)

        if state > 1 and state ~= 999 then

            processJobChange()

            return

        end

    end

    local talkBox = NPCTalkBox:new(width, height)
    talkBox:addString(stringBase + 1)

    player:talk(npc, talkBox)
    player:disposeTalk()

end
