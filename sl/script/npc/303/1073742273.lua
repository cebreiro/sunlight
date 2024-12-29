
return function (system, npc, player, sequence)

    local width = 400
    local height = 250
    local stringBase = 100100

    local function handleJobChangeQuest(quest)

        local questId = quest:getId()
        local step = quest:getFlag(1)

        if step == 3 then
            
            if sequence == 0 then

                local talkBox = NPCTalkBox:new(width, height)
                talkBox:addString(stringBase + 10)
                talkBox:addString(stringBase + 11)

                player:talk(npc, talkBox)

                return

            elseif sequence == 1 then

                local questChange = QuestChange:new()
                questChange:setFlag(1, 4)

                player:changeQuest(questId, questChange)

            end

        elseif step >= 4 and step < 995 then

            local talkBox = NPCTalkBox:new(width, height)
            talkBox:addString(stringBase + 12)

            player:talk(npc, talkBox)


        elseif step == 995 then

            if sequence == 0 then

                local talkBox = NPCTalkBox:new(width, height)
                talkBox:addString(stringBase + 13)

                player:talk(npc, talkBox)

                return

            elseif sequence == 1 then

                local itemId = quest:getFlag(5)

                if player:addItem(itemId, 1) then

                    local questChange = QuestChange:new()
                    questChange:setFlag(1, 996)

                    player:changeQuest(questId, questChange)

                end

            end

        elseif step >= 996 then

            local talkBox = NPCTalkBox:new(width, height)
            talkBox:addString(stringBase + 14)

            player:talk(npc, talkBox)

        end


        player:disposeTalk()

    end

    local list = {}
    table.insert(list, player:findQuest(1101))
    table.insert(list, player:findQuest(1102))

    for i, jobChangeQuest in ipairs(list) do

        local step = jobChangeQuest:getFlag(1)

        if step > 2 and step ~= 999 then

            handleJobChangeQuest(jobChangeQuest)

            return

        end

    end

    local talkBox = NPCTalkBox:new(width, height)
    talkBox:addString(stringBase + 1)

    player:talk(npc, talkBox)
    player:disposeTalk()

end
