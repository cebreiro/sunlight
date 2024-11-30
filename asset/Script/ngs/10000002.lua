-- Ngs         : 10000002
-- Description : [공용] 랜덤 대사 출력 (타겟[0]=스트링아이디베이스, 타겟[1]-랜덤범위)

function talkRandom (npc, player, stringBase, range)

    local string = math.random(stringBase + 1, stringBase + range)

    local talkBox = NPCTalkBox:new(400, 150)
    talkBox:addString(string)

    player:talk(npc, talkBox)

end

return {
    talkRandom = talkRandom
}
