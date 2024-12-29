
local function handleMonsterKill(system, player, quest, monsterId)

    local step = quest:getFlag(1)
    local targetMonsterId = quest:getFlag(2)
    local requiredKillCount = quest:getFlag(3)
    local killCount = quest:getFlag(4)

    if step == 5 and monsterId == targetMonsterId then

        local newKillCount = killCount + 1

        local questChange = QuestChange:new()
        questChange:setFlag(4, newKillCount)

        if newKillCount >= requiredKillCount then

            questChange:setFlag(1, 995)

            local eventScript = EventScript:new()
            eventScript:addString(455)
            eventScript:addString(14001100 + 52)

            player:show(eventScript)

        end

        player:changeQuest(quest:getId(), questChange)

    end

end

local Quest = {}
Quest.handleMonsterKill = handleMonsterKill

return Quest
