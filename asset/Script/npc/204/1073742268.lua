-- Zone  : 204
-- Stage : 10000
-- Name  : 미용실의 미치르

return function (system, npc, player, sequence)

    local width = 400
    local height = 150

    if sequence == 0 then
        local talkBox = NPCTalkBox:new(width, height)
        talkBox:addString(10401) -- 흐음... 머리를 다듬으러 온 건가요?

        player:talk(npc, talkBox)

        return

    elseif sequence == 1 then

        player:openShop(npc)

    end

    player:disposeTalk()

end
