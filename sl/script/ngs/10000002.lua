function talkRandom (npc, player, stringBase, range)

    local string = math.random(stringBase + 1, stringBase + range)

    local talkBox = NPCTalkBox:new(400, 150)
    talkBox:addString(string)

    player:talk(npc, talkBox)

end

return {
    talkRandom = talkRandom
}
