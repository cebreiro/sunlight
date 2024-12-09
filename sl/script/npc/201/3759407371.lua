-- Zone  : 201
-- Stage : 10020
-- Name  : 베르베르 (의상점)
-- Nes   : 100055, 100039, 100049
-- Role  : 디자이너 전직 퀘스트, 펫 2 -> 3 퀘스트

return function (system, npc, player, sequence)

    local width = 400
    local height = 250
    local stringBase = 4100

    local designerQuestId = 203
    local designerQuest = player:findQuest(designerQuestId)

    if designerQuest ~= nil then

        if designerQuest:getState() == 0 then

            local step = designerQuest:getFlag(1)

            if step == 0 then

                if sequence == 0 then

                    local talkBox = NPCTalkBox:new(width, height)
                    talkBox:addString(30002)

                    player:talk(npc, talkBox)

                    return

                elseif sequence == 1 then

                    local questChange = QuestChange:new()
                    questChange:setFlag(1, 1)

                    player:changeQuest(designerQuestId, questChange)

                    -- 실타레
                    player:removeInventoryItem(5050027, 1)

                end

            elseif step == 1 then

                local talkBox = NPCTalkBox:new(width, height)
                talkBox:addString(30003)

                player:talk(npc, talkBox)

            end

            player:disposeTalk()

            return

        end

    end

    -- TODO: 펫 2 -> 3 퀘스트, 100049
    local petQuestId = 113



    local ngs = require("10000002")
    ngs.talkRandom(npc, player, stringBase, 3)

    player:disposeTalk()

end
