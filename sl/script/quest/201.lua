
local function handleMonsterKill(system, player, quest, monsterId)

    local step = quest:getFlag(1)
    local killCount = quest:getFlag(3)
    local targetMonsterId = quest:getFlag(5)

    if step == 10 and monsterId == targetMonsterId then

        local itemId = 5050019

        if player:hasInventoryItem(itemId, 1) then

            return

        end

        local newKillCount = killCount + 1

        local questChange = QuestChange:new()
        questChange:setFlag(3, newKillCount)

        if newKillCount >= 10 then

            if player:addItem(itemId, 1) then

                questChange:setFlag(1, 20)

                local eventScript = EventScript:new()
                eventScript:addString(455)
                eventScript:addString(60066) -- 칼빈슨의 단검을 획득했습니다.

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
