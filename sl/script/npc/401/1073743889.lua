
return function (system, npc, player, sequence)

    local width = 400
    local height = 150

    local talkBox = NPCTalkBox:new(width, height)
    talkBox:addString(3118) -- 특별히 볼 일이 없으면 방해하지 마~! 

    player:talk(npc, talkBox)

    player:disposeTalk()

end
