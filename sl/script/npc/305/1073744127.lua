
return function (system, npc, player, sequence)

    local width = 400
    local height = 250
    local stringBase = 100400

    local function handleJobChangeQuest(quest)

        local questId = quest:getId()
        local step = quest:getFlag(1)

        if step == 2 then

            if sequence == 0 then

                local talkBox = NPCTalkBox:new(width, height)
                talkBox:addString(stringBase + 10)
                talkBox:addString(stringBase + 11)

                player:talk(npc, talkBox)

                return

            elseif sequence == 1 then

                local questChange = QuestChange:new()
                questChange:setFlag(1, 3)

                player:changeQuest(questId, questChange)

            end

        elseif step >= 3 and step < 996 then

            local talkBox = NPCTalkBox:new(width, height)
            talkBox:addString(stringBase + 12)

            player:talk(npc, talkBox)


        elseif step == 996 then

            if sequence == 0 then

                local talkBox = NPCTalkBox:new(width, height)
                talkBox:addString(stringBase + 13)
                talkBox:addString(stringBase + 14)

                player:talk(npc, talkBox)

                return

            elseif sequence == 1 then

                local questChange = QuestChange:new()
                questChange:setFlag(1, 997)
                questChange:setState(1)

                player:changeQuest(questId, questChange)

            end


        elseif step == 997 then

            local talkBox = NPCTalkBox:new(width, height)
            talkBox:addString(stringBase + 15)

            player:talk(npc, talkBox)

        end

        player:disposeTalk()

    end

    local list = {}
    table.insert(list, player:findQuest(1201))
    table.insert(list, player:findQuest(1203))

    for i, jobChangeQuest in ipairs(list) do

        local step = jobChangeQuest:getFlag(1)

        if step > 1 and step ~= 999 then

            handleJobChangeQuest(jobChangeQuest)

            return

        end

    end


    local talkBox = NPCTalkBox:new(width, height)
    talkBox:addString(stringBase + 1)
    talkBox:addString(stringBase + 2)

    player:talk(npc, talkBox)
    player:disposeTalk()

end
