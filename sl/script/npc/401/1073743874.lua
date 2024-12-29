
return function (system, npc, player, sequence)

    local width = 400
    local height = 150

    if sequence == 0 then
        local talkBox = NPCTalkBox:new(width, height)
        talkBox:addString(3122) -- 자~~ 골라보세요~ 초보 모험가들에게 꼭 필요한 물건만 준비해 놓았습니다.

        player:talk(npc, talkBox)

        return

    elseif sequence == 1 then

        player:openShop(npc)

    end

    player:disposeTalk()

end
