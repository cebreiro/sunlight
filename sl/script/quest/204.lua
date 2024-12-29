
local function handleMonsterKill(system, player, quest, monsterId)

    local step = quest:getFlag(2)
    local targetMonsterId = quest:getFlag(4)
    local killCount = quest:getFlag(5)

    if step == 1 and monsterId == targetMonsterId then

        local itemId = 5050022 -- 토파즈
        local itemQuantity = 3

        if player:hasInventoryItem(itemId, itemQuantity) then

            return

        end

        local newKillCount = killCount + 1

        local questChange = QuestChange:new()
        questChange:setFlag(5, newKillCount)

        if newKillCount >= 5 then

            if player:addItem(itemId, 1) then

                local eventScript = EventScript:new()
                eventScript:addString(455)
                eventScript:addString(60112)

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
