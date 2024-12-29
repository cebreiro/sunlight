return function (system, npc, player, sequence)

    local width = 400
    local height = 200

    local swordWarriorQuestId = 1001
    local swordWarriorQuest = player:findQuest(swordWarriorQuestId)

    if swordWarriorQuest ~= nil and swordWarriorQuest:getFlag(1) == 3 then

        if sequence == 0 then

            local talkBox = NPCTalkBox:new(width, height)
            talkBox:addString(10820) -- 어서 오세요. 저는 잡화점을 경영하고 있는 호레랍니다~ 부디 저희 가게를 부담 없이! 걱정 없이! 이용해주세요~
            talkBox:addMenu(10821, 10822, 1) -- 오버기어의 설계도에 대해 물어본다

            player:talk(npc, talkBox)

            return

        elseif sequence == 1 then

            local talkBox = NPCTalkBox:new(width, height)
            talkBox:addString(10830) -- 지금 뭔 소리 하시는 걸까나요? 설계도? 오버기어로이드? 전혀 금시초문인걸요.
            talkBox:addString(10831) -- 그러고 보면 <#08BB00>사적왕<#000000>, 아직도 진짜 정체가 드러나지 않았다죠?
            talkBox:addMenu(10832, 10833, 1) -- <#000000><B>뭔가 기억나는 것이 없는지 캐묻는다.</B>

            player:talk(npc, talkBox)

            return

        elseif sequence == 2 then

            local questChange = QuestChange:new()
            questChange:setFlag(7, 1)

            player:changeQuest(swordWarriorQuestId, questChange)

            if swordWarriorQuest:getFlag(8) == 1 then -- 애쉬타와 대화를 한 경우

                local talkBox = NPCTalkBox:new(width, height)
                talkBox:addString(10841) -- 그게 설계도인지 아니면 자기 속옷쯤 되는지는 전혀 모르겠지만 말이죠. 손님이 생각하시는 '수상한 사람'의 조건에는 들어맞는 것 같지 않아요?
                talkBox:addString(10842) -- 아, 사적스파이들의 거점은 <#FF4200>무너진 마을<#000000>이라고 하더군요.

                player:talk(npc, talkBox)

            else

                local talkBox = NPCTalkBox:new(width, height)
                talkBox:addString(10840) -- 저보다는 <#FF4200>엔트란스의 잡화상<#000000>을 운영하는 <#FF4200>애쉬타<#000000> 씨에게 물어보는 게 나을 거에요.

                player:talk(npc, talkBox)

            end

        end

        player:disposeTalk()

        return

    end

    local talkBox = NPCTalkBox:new(width, height)
    talkBox:addString(10810) -- 어서 오세요. 저는 잡화점을 경영하고 있는 호레랍니다~ 부디 저희 가게를 부담 없이! 걱정 없이! 이용해주세요~

    player:talk(npc, talkBox)
    player:disposeTalk()

end
