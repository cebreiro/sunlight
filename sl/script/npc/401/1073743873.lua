

return function (system, npc, player, sequence)

    local noviceQuestId = 200
    local width = 400
    local height = 250

    local questStepflagIndex = 0
    local stepDeliver = 20

    local deliverStepStateIndex = 4
    local deliverStepSuccess = 10

    local questItemId = 5050018
    local questItemCount = 3

    local quest = player:findQuest(noviceQuestId)

    if quest ~= nil and quest:getFlag(questStepflagIndex) == stepDeliver and quest:getFlag(deliverStepStateIndex) ~= deliverStepSuccess then

        if quest:isExpired() then

            local talkBox = NPCTalkBox:new(width, height)
            talkBox:addString(3113) -- 그 긴급 환자는 이미 큰 병원으로 송환되었습니다.

            player:talk(npc, talkBox)

        else

            if player:hasInventoryItem(questItemId, questItemCount) then

                local questChange = QuestChange:new()
                questChange:setFlag(deliverStepStateIndex, deliverStepSuccess)

                player:changeQuest(noviceQuestId, questChange)
                player:removeInventoryItem(questItemId, questItemCount)


                local eventScript = EventScript:new()
                eventScript:addStringWithInt(512, questItemId) -- <#FF0000><$ITEM%d>
                eventScript:addStringWithInt(508, questItemCount) --  %d개가 회수되었습니다.

                player:show(eventScript)


                local talkBox = NPCTalkBox:new(width, height)
                talkBox:addString(3110) -- 정말 감사합니다. 이 약초 덕분에 응급실의 환자를 큰 병원으로 보내지 않고도 치료 할 수 있겠군요. 

                player:talk(npc, talkBox)

            else

                local talkBox = NPCTalkBox:new(width, height)
                talkBox:addString(3109) -- 아, 초보자 캠프에서 오신 분이시군요? 

                player:talk(npc, talkBox)

            end

        end

    else
        local healingCost = player:getLevel() * 10

        if sequence == 0 then

            local talkBox = NPCTalkBox:new(width, height)
            talkBox:addString(3102) -- 안녕하세요, 여기는 판도라 초보자 캠프 출장 병원입니다

            talkBox:addStringWithInt(3103, healingCost) -- 상처 치료에 필요한 비용은 <#089900>%d판드<#000000> 입니다.

            talkBox:addMenu(3104, 3105, 1) -- 지금 치료한다
            talkBox:addMenu(3106, 3107, 2) -- 다음에 치료한다

            player:talk(npc, talkBox)

            return

        elseif sequence == 1 then

            local selection = player:getSelection()
            
            if selection == 1 then

                if player:charge(healingCost) then

                    player:recoverHP()


                    local es1 = EventScript:new()
                    es1:addStringWithInt(514, healingCost) -- <#FF0000>%d 판드를 지불했습니다.

                    player:show(es1)


                    local es2 = EventScript:new()
                    es2:addString(60012, healingCost) -- <#00B4FF>HP가 회복되었습니다

                    player:show(es2)

                end

                if quest ~= nil and quest:getState() == 1 then

                    local talkBox = NPCTalkBox:new(width, height)
                    talkBox:addString(3101) -- <B><#3E1CBA>간호원 푸치<#000000><BR></B>
                    talkBox:addString(3112) -- 참, 초보자 캠프의 프란시스 교관님은 요즘 어떠신지 궁금하네요.

                    player:talk(npc, talkBox)

                end

            elseif selection == 2 then
                local talkBox = NPCTalkBox:new(width, height)
                talkBox:addString(3108) -- 그럼 언제든지 상처를 입으면 저를 찾아오세요.

                player:talk(npc, talkBox)

            end
        end

    end

    player:disposeTalk()

end
