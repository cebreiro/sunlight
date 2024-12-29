
local function handleMonsterKill(system, player, quest, monsterId)

    local step = quest:getFlag(1)

    -- 초원 일개미
    local targetMonsterId1 = quest:getFlag(2)
    local killCount1 = quest:getFlag(3)

    if step == 10 and monsterId == targetMonsterId1 then

        local itemId = 5050029 -- 광석

        if not player:hasInventoryItem(itemId, 1) then

            local newKillCount = killCount1 + 1

            local questChange = QuestChange:new()
            questChange:setFlag(3, newKillCount)

            if newKillCount >= 10 then

                if player:addItem(itemId, 1) then

                    questChange:setFlag(1, 20)

                    local eventScript = EventScript:new()
                    eventScript:addString(455)
                    eventScript:addString(60080)

                    player:show(eventScript)

                else

                    local eventScript = EventScript:new()
                    eventScript:addString(332) -- 인벤토리의 여유 공간이 부족합니다.

                    player:show(eventScript)

                end

            end

            player:changeQuest(quest:getId(), questChange)

        end

    end

    -- 뿅망치 선생
    local targetMonsterId2 = quest:getFlag(4)
    local killCount2 = quest:getFlag(5)

    if monsterId == targetMonsterId2 then

        local itemId = 5050021 -- 그린 토파즈

        if not player:hasInventoryItem(itemId, 1) then

            local newKillCount = killCount2 + 1

            local questChange = QuestChange:new()
            questChange:setFlag(5, newKillCount)

            if newKillCount >= 10 then

                if player:addItem(itemId, 1) then

                    local eventScript = EventScript:new()
                    eventScript:addString(455)
                    eventScript:addString(60081)

                    player:show(eventScript)

                else

                    local eventScript = EventScript:new()
                    eventScript:addString(332) -- 인벤토리의 여유 공간이 부족합니다.

                    player:show(eventScript)

                end

            end

            player:changeQuest(quest:getId(), questChange)

        end

    end

end

local Quest = {}
Quest.handleMonsterKill = handleMonsterKill

return Quest
