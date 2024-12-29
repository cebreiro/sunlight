return function (system, npc, player, sequence)

    local width = 400
    local height = 250
    local stringBase = 11200

    local quest = player:findQuest(1104)
    if quest ~= nil then

        local questId = quest:getId()
        local step = quest:getFlag(1)

        if step > 1 and step ~= 999 then

            if step == 2 then -- 전직 퀘스트를 받고 처음

                if sequence == 0 then

                    local talkBox = NPCTalkBox:new(width, height)
                    talkBox:addString(stringBase + 10)
                    talkBox:addString(stringBase + 11)

                    player:talk(npc, talkBox)

                    return

                elseif sequence == 1 then

                    if player:addItem(5050006, 1) then

                        local questChange = QuestChange:new()
                        questChange:setFlag(1, 3)
                        questChange:setFlag(2, 5050006)
                        questChange:setFlag(3, 5050007)
                        questChange:setFlag(4, 5050008)

                        player:changeQuest(questId, questChange)

                    end

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
                    questChange:setState(1)
                    questChange:setFlag(1, 997)

                    player:changeQuest(questId, questChange)

                end

            elseif step == 997 then

                local talkBox = NPCTalkBox:new(width, height)
                talkBox:addString(stringBase + 15)

                player:talk(npc, talkBox)

            end


            player:disposeTalk()

            return

        end

    end


    local talkBox = NPCTalkBox:new(width, height)
    talkBox:addString(stringBase + 1)
    talkBox:addString(stringBase + 2)
    talkBox:addString(stringBase + 3)

    player:talk(npc, talkBox)
    player:disposeTalk()

end
