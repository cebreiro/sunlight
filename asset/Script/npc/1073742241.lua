-- Zone  : 204
-- Stage : 10000
-- Name  : 창고담당 다일

return function (system, npc, player, sequence)

    local width = 400
    local height = 150
    local levelCondition = 3

    if sequence == 0 then
        local talkBox = NPCTalkBox:new(width, height)
        talkBox:addString(3031) -- 여러분의 무거운 짐을 대신 맡아드리고 있습니다- 어때요, 창고를 한 번 써 보시겠어요?
        talkBox:addMenu(3033, 3034, 1) -- <#000000><B>창고를 이용한다
        talkBox:addMenu(203, 204, 2) -- <#000000><B>대화를 끝낸다</B>

        player:talk(npc, talkBox)

        return

    elseif sequence == 1 then

        if player:getSelection() == 1 then

            if player:getNoviceJobLevel() > levelCondition then

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
