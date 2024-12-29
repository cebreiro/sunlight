return function (system, npc, player, sequence)

    local width = 400
    local height = 250
    local stringBase = 33900

    local quest = player:findQuest(1200)
    if quest ~= nil then

        local step = quest:getFlag(1)

        if step == 1 then

            local jobChangeQuestId = quest:getFlag(0)

            if jobChangeQuestId ~= 1204 then

                local talkBox = NPCTalkBox:new(width, height)
                talkBox:addString(stringBase + 10)

                player:talk(npc, talkBox)
                player:disposeTalk()

                return

            end

            local talkBox = NPCTalkBox:new(width, height)
            talkBox:addString(stringBase + 80)

            player:talk(npc, talkBox)
            player:disposeTalk()

            return

        end

    end

    if not player:isMagician() then

        local talkBox = NPCTalkBox:new(width, height)
        talkBox:addString(stringBase + 50)

        player:talk(npc, talkBox)
        player:disposeTalk()

        return

    end

    if not player:isNovice() then

        local talkBox = NPCTalkBox:new(width, height)
        talkBox:addString(stringBase + 60)

        player:talk(npc, talkBox)
        player:disposeTalk()

        return

    end

    local talkBox = NPCTalkBox:new(width, height)
    talkBox:addString(stringBase + 70)
    talkBox:addString(stringBase + 71)

    player:talk(npc, talkBox)
    player:disposeTalk()

    return

end
