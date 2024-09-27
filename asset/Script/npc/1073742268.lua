-- Zone  : 204
-- Stage : 10000
-- Name  : 미용실의 미치르

return function (system, npc, player, sequence)

    local width = 400
    local height = 150

    if sequence == 0 then
        local talkBox = NPCTalkBox:new(width, height)
        talkBox:addString(24023) -- 어서오세요! 트랜돌 최고의 헤어 디자이너인 미치르의 가게에 잘 오셨습니다

        player:talk(npc, talkBox)

        return

    elseif sequence == 1 then

        player:openShop(npc)

    end

    player:disposeTalk()

end
