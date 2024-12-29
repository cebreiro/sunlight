
local function handleMonsterKill(system, player, quest, monsterId)

    local step = quest:getFlag(4)
    local targetMonsterId = quest:getFlag(5)
    local killCount = quest:getFlag(6)

    if step == 1 and monsterId == targetMonsterId then

        local itemId = 5050026 -- 헤벌레 실크
        local itemQuantity = 5

        if player:hasInventoryItem(itemId, itemQuantity) then

            return

        end

        local newKillCount = killCount + 1

        local questChange = QuestChange:new()
        questChange:setFlag(6, newKillCount)

        if newKillCount >= 5 then

            if player:addItem(itemId, 1) then

                local eventScript = EventScript:new()
                eventScript:addString(455)
                eventScript:addString(60103)

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

local Quest = {}
Quest.handleMonsterKill = handleMonsterKill

return Quest
