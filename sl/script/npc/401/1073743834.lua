
return function (system, npc, player, sequence)

    local width = 400
    local height = 200

    if sequence == 0 then

        local talkBox = NPCTalkBox:new(width, height)
        talkBox:addString(3120) -- 필요한 물건을 골라주세요~!

        talkBox:addMenu(55, 56, 1) -- 상점을 이용한다
        talkBox:addMenu(57, 58, 2) -- 취소한다

        player:talk(npc, talkBox)

        return

    elseif sequence == 1 then

        if player:getSelection() == 1 then

            player:openShop(npc)

        end

    end

    player:disposeTalk()

end
