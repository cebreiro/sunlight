-- Zone  : 204
-- Stage : 10000
-- Nes   : 100044, 100029
-- Name  : 미용실의 미치르
-- Role  : 미용, 디자이너 전직 퀘스트

return function (system, npc, player, sequence)

    local width = 400
    local height = 250
    local stringBase = 10400

    local designerQuestId = 203
    local designerQuest = player:findQuest(designerQuestId)

    if designerQuest ~= nil then

        if designerQuest:getState() == 0 then

            local step = designerQuest:getFlag(3)

            if step == 0 then

                local talkBox = NPCTalkBox:new(width, height)
                talkBox:addString(24023)

                player:talk(npc, talkBox)

            elseif step == 1 then

                local talkBox = NPCTalkBox:new(width, height)
                talkBox:addString(24024)

                player:talk(npc, talkBox)

            end

            player:disposeTalk()

            return

        end

    end

    -- TODO: 투구 먹물 문어 퀘스트

    if sequence == 0 then

        local talkBox = NPCTalkBox:new(width, height)
        talkBox:addString(stringBase)

        player:talk(npc, talkBox)

        return

    elseif sequence == 1 then

        player:openShop(npc)

    end

    player:disposeTalk()

end
