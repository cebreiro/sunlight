function useStorage (system, npc, player, sequence, stringBase)

    local width = 400
    local height = 150
    local levelCondition = 5

    if sequence == 0 then
        local talkBox = NPCTalkBox:new(width, height)
        talkBox:addString(stringBase + 1)
        talkBox:addMenu(stringBase + 3, stringBase + 4, 1)
        talkBox:addMenu(201, 202, 999) -- <#000000><B>대화를 끝낸다</B>

        player:talk(npc, talkBox)

        return

    elseif sequence == 1 then

        if player:getSelection() == 1 then

            if player:getNoviceJobLevel() >= levelCondition then

                player:openAccountStorage()

            else

                local talkBox = NPCTalkBox:new(width, height)
                talkBox:addString(3032) -- 창고를 이용하시기엔 아직 레벨이 부족하시네요. 다음에 다시 오셔야겠습니다.

                player:talk(npc, talkBox)

            end

        end

    end

    player:disposeTalk()

end

return {
    useStorage = useStorage
}
