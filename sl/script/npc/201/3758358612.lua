return function (system, npc, player, sequence)

    local width = 400
    local height = 250
    local stringBase = 4000

    local weaponSmithQuestId = 202
    local weaponSmithQuest = player:findQuest(weaponSmithQuestId)

    if weaponSmithQuest ~= nil then

        if weaponSmithQuest:getState() == 0 then

            local step = weaponSmithQuest:getFlag(0)

            if step == 20 then

                if sequence == 0 then

                    local talkBox = NPCTalkBox:new(width, height)
                    talkBox:addString(21019)

                    player:talk(npc, talkBox)

                    return

                elseif sequence == 1 then

                    player:removeInventoryItem(5050020, 1) -- 윈드엣지

                    local questChange = QuestChange:new()
                    questChange:setFlag(0, 30)

                    player:changeQuest(weaponSmithQuestId, questChange)

                end

            elseif step < 20 then

                local talkBox = NPCTalkBox:new(width, height)
                talkBox:addString(21018)

                player:talk(npc, talkBox)

            elseif step > 20 then

                local talkBox = NPCTalkBox:new(width, height)
                talkBox:addString(21020)

                player:talk(npc, talkBox)

            end

            player:disposeTalk()

            return

        end

    end


    -- TODO: 겸직 퀘스트


    local ngs = require("10000002")
    ngs.talkRandom(npc, player, stringBase, 3)

    player:disposeTalk()

end
