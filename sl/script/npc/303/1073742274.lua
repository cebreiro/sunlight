
return function (system, npc, player, sequence)

    local width = 400
    local height = 250
    local stringBase = 100200

    local quest = player:findQuest(1104)
    if quest ~= nil then

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

                player:removeInventoryItemAll(5050006)

                if player:addItem(5050007, 1) then

                    local questChange = QuestChange:new()
                    questChange:setFlag(1, 4)

                    player:changeQuest(questId, questChange)

                end

            end

            player:disposeTalk()

            return

        elseif step == 4 then

            local talkBox = NPCTalkBox:new(width, height)
            talkBox:addString(stringBase + 12)

            player:talk(npc, talkBox)
            player:disposeTalk()

            return

        end

    end

    local talkBox = NPCTalkBox:new(width, height)
    talkBox:addString(stringBase + 1)

    player:talk(npc, talkBox)
    player:disposeTalk()

end
