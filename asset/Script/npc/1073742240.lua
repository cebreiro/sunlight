-- Zone  : 204
-- Stage : 10000
-- Name  : 창고담당 나스

return function (system, npc, player, sequence)

    local width = 400
    local height = 150
    local levelCondition = 3

    if sequence == 0 then
        local talkBox = NPCTalkBox:new(width, height)
        talkBox:addString(3041) -- 흐음, 창고를 쓸 건가요?
        talkBox:addMenu(3043, 3044, 1) -- <#000000><B>창고를 이용한다
        talkBox:addMenu(203, 204, 2) -- <#000000><B>대화를 끝낸다</B>

        player:talk(npc, talkBox)

        return

    elseif sequence == 1 then

        if player:getSelection() == 1 then

            if player:getNoviceJobLevel() > levelCondition then

                player:openAccountStorage()

            else

                local talkBox = NPCTalkBox:new(width, height)
                talkBox:addString(3042) -- 뭐야, 아직 창고를 이용할 만한 레벨이 되지도 않은 상태잖아? 좀 더 수련을 쌓고 와야겠네요.

                player:talk(npc, talkBox)

            end

        end

    end

    player:disposeTalk()

end
