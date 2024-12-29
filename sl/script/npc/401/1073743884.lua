
return function (system, npc, player, sequence)

    local width = 400
    local height = 150

    local talkBox = NPCTalkBox:new(width, height)
    talkBox:addString(3083) -- <말 시키지마세욧!

    player:talk(npc, talkBox)

    player:disposeTalk()

end
