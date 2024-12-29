return function (system, npc, player, sequence)

    local width = 400
    local height = 250
    local stringBase = 4200

    local chefQuestId = 201
    local chefQuest = player:findQuest(201)

    if chefQuest ~= nil then

        local state = chefQuest:getState()
        if state == 0 then

            local phase = chefQuest:getFlag(0)

            if phase == 20 then

                if sequence == 0 then

                    local talkBox = NPCTalkBox:new(width, height)
                    talkBox:addString(24009) -- 응?  아니 이것은 ‘바다라 돈까스’ 아닌가요? 이걸 왜 나에게?

                    player:talk(npc, talkBox)

                    return

                elseif sequence == 1 then

                    player:removeInventoryItem(5050025, 1) -- 돈까스

                    local questChange = QuestChange:new()
                    questChange:setFlag(0, 30)

                    player:changeQuest(chefQuestId, questChange)

                end

                player:disposeTalk()

                return

            elseif phase > 20 then

                local talkBox = NPCTalkBox:new(width, height)
                talkBox:addString(24010) -- 쉐프링님께 돈가스 잘 먹겠다고 꼭 전해주세요.~!

                player:talk(npc, talkBox)
                player:disposeTalk()

                return

            end


        end

    end

    local ngs = require("10000002")
    ngs.talkRandom(npc, player, stringBase, 3)

    player:disposeTalk()

end
