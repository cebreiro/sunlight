-- Zone  : 202
-- Stage : 10029
-- Name  : 애쉬타 (잡화점)
-- Nes   : 100097
-- Role  : 검투사 전직 퀘스트

return function (system, npc, player, sequence)

    local width = 400
    local height = 200

    local swordWarriorQuestId = 1001
    local swordWarriorQuest = player:findQuest(swordWarriorQuestId)

    if swordWarriorQuest ~= nil and swordWarriorQuest:getFlag(1) == 3 then

        if sequence == 0 then

            local talkBox = NPCTalkBox:new(width, height)
            talkBox:addString(10720) -- 안녕하신가~ 나는 이 잡화점을 지배! 가 아니라 단지 경영하고 있을 뿐인 애쉬타라고 해. 뭔가 볼 일이 있으면 사사삭 잽싸게 보라고.
            talkBox:addMenu(10721, 10722, 1) -- 오버기어의 설계도에 대해 물어본다

            player:talk(npc, talkBox)

            return

        elseif sequence == 1 then

            local talkBox = NPCTalkBox:new(width, height)
            talkBox:addString(10730) -- 응? 오버기어로이드의 설계도? 게다가 신형? 최첨단? 대체 뭐야? 말이 좋아 오버기어로이드지, 어디 그거 만드는 일이 그렇게 쉬운 줄 아나.
            talkBox:addString(10731) -- 요즘 사람들은 다들 오버기어 오버기어, 입으로 말할 줄만 알고 그거 만드는 게 얼마나 힘든지는 조금도 모르니 원... 
            talkBox:addMenu(10732, 10733, 1) -- <#000000><B>뭔가 기억나는 것이 없는지 캐묻는다.</B>

            player:talk(npc, talkBox)

            return

        elseif sequence == 2 then

            local questChange = QuestChange:new()
            questChange:setFlag(8, 1)

            player:changeQuest(swordWarriorQuestId, questChange)

            if swordWarriorQuest:getFlag(7) == 1 then -- 호레와 대화를 한 경우

                local talkBox = NPCTalkBox:new(width, height)
                talkBox:addString(10741) -- <#FF4200>무너진 마을<#000000> 쪽으로 가는 것 같더라던데…
                talkBox:addString(10742) -- 당장 가보면 잡을 수 있을지도 몰라. 물론 <#FF4200>사적스파이<#000000>라고 해도 머릿수도 한 두 명이 아니니 어느 녀석이 범인인지는 일일히 잡아서 몸을 뒤져보지 않으면 모르겠지만...

                player:talk(npc, talkBox)

            else

                local talkBox = NPCTalkBox:new(width, height)
                talkBox:addString(10740) -- 나는 잘 모르겠어. 아무래도 <#FF4200>벨슈로트 잡화상<#000000>에 있는 <#FF4200>호레<#000000> 녀석에게 가서 물어보는 게 낫지 않을라나?

                player:talk(npc, talkBox)

            end

        end

        player:disposeTalk()

        return

    end

    local talkBox = NPCTalkBox:new(width, height)
    talkBox:addString(10710) -- 안녕하신가~ 나는 이 잡화점을 지배! 가 아니라 단지 경영하고 있을 뿐인 애쉬타라고 해. 뭔가 볼 일이 있으면 사사삭 잽싸게 보라고.

    player:talk(npc, talkBox)
    player:disposeTalk()

end
