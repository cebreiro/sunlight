
local function handleMonsterKill(system, player, quest, monsterId)

    local step = quest:getFlag(1)
    local targetMonsterId = quest:getFlag(2)
    local requiredKillCount = quest:getFlag(3)
    local killCount = quest:getFlag(4)

    if step == 3 and monsterId == targetMonsterId then

        local itemId = quest:getFlag(5)

        if player:hasInventoryItem(itemId, 1) then

            return

        end

        local newKillCount = killCount + 1

        local questChange = QuestChange:new()
        questChange:setFlag(4, newKillCount)

        if newKillCount >= requiredKillCount then

            if player:addItem(itemId, 1) then

                questChange:setFlag(1, 996)

                local eventScript = EventScript:new()
                eventScript:addString(455)
                eventScript:addString(14000100 + 44)

                player:show(eventScript)

            end

        end

        player:changeQuest(quest:getId(), questChange)

    end

end

local Quest = {}
Quest.handleMonsterKill = handleMonsterKill

return Quest
