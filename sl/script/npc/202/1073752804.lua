return function (system, npc, player, sequence)

    local width = 400
    local height = 250
    local stringBase = 10500

    local function warp()

        player:changeZoneWithStage(306, 10002, 5038, 4909)

    end

    local function handleJobChangeQuest(quest)

        local questId = quest:getId()
        local step = quest:getFlag(1)

        if step == 3 then

            if sequence == 0 then

                local talkBox = NPCTalkBox:new(width, 200)

                if player:isMale() then

                    talkBox:addString(stringBase + 10)

                else

                    talkBox:addString(stringBase + 12)

                end

                talkBox:addMenu(stringBase + 20, stringBase + 21, 1)
                talkBox:addMenu(stringBase + 22, stringBase + 23, 2)

                player:talk(npc, talkBox)

                return

            elseif sequence == 1 then

                local questChange = QuestChange:new()
                questChange:setFlag(1, 4)

                player:changeQuest(questId, questChange)

                if player:getSelection() == 1 then

                    warp()

                end

            end

        elseif step >= 4 and step < 996 then

            if sequence == 0 then

                local talkBox = NPCTalkBox:new(width, 200)

                if player:isMale() then

                    talkBox:addString(stringBase + 11)

                else

                    talkBox:addString(stringBase + 13)

                end

                talkBox:addMenu(stringBase + 20, stringBase + 21, 1)
                talkBox:addMenu(stringBase + 22, stringBase + 23, 2)

                player:talk(npc, talkBox)

                return

            elseif sequence == 1 then

                if player:getSelection() == 1 then

                    warp()

                end

            end

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

    if player:isMale() then

        local talkBox = NPCTalkBox:new(width, height)
        talkBox:addString(stringBase + 1)
        talkBox:addString(stringBase + 2)

        player:talk(npc, talkBox)
        player:disposeTalk()

    else

        local talkBox = NPCTalkBox:new(width, height)
        talkBox:addString(stringBase + 3)
        talkBox:addString(stringBase + 4)

        player:talk(npc, talkBox)
        player:disposeTalk()

    end

end
