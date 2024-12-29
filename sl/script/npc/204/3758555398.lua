return function (system, npc, player, sequence)

    local width = 400
    local height = 250
    local stringBase = 4100

    local designerQuestId = 203
    local designerQuest = player:findQuest(designerQuestId)

    if designerQuest ~= nil then

        if designerQuest:getState() == 0 then

            local step = designerQuest:getFlag(3)

            if step == 1 then

                local talkBox = NPCTalkBox:new(width, height)
                talkBox:addString(24014)

                player:talk(npc, talkBox)
                player:disposeTalk()

                return

            end

            local step2 = designerQuest:getFlag(4)

            if step2 == 0 then

                if sequence == 0 then

                    local talkBox = NPCTalkBox:new(width, height)
                    talkBox:addString(24013)

                    player:talk(npc, talkBox)

                    return

                elseif sequence == 1 then

                    local questChange = QuestChange:new()
                    questChange:setFlag(4, 1)
                    questChange:setFlag(5, 6901) -- 헤벌레 id

                    player:changeQuest(designerQuestId, questChange)

                    -- 실타레
                    player:removeInventoryItem(5050027, 1)

                end

            elseif step2 == 1 then

                local requiredItemId = 5050026 -- 헤벌레 실크
                local requiredItemQuantity = 5

                if player:hasInventoryItem(requiredItemId, requiredItemQuantity) then

                    player:removeInventoryItemAll(requiredItemId)

                    local questChange = QuestChange:new()
                    questChange:setFlag(3, 1)

                    player:changeQuest(designerQuestId, questChange)

                else

                    local talkBox = NPCTalkBox:new(width, height)
                    talkBox:addString(24015)

                    player:talk(npc, talkBox)

                end

            end

            player:disposeTalk()

            return

        end

    end

    local talkBox = NPCTalkBox:new(width, height)
    talkBox:addString(stringBase)

    player:talk(npc, talkBox)
    player:disposeTalk()

end
