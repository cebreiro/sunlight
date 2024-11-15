-- Zone  : 401
-- Stage : 10000
-- Name  : 푸치
-- Quest : 200

return function (system, npc, player, sequence)

    local noviceQuestId = 200
    local width = 400
    local height = 150

    local questStepflagIndex = 0
    local stepDeliver = 20

    local deliverStepStateIndex = 4
    local deliverStepSuccess = 10

    local questItemId = 5050018

    local quest = player:findQuest(noviceQuestId)

    if quest ~= nil and quest:getFlag(questStepflagIndex) == stepDeliver and quest:getFlag(deliverStepStateIndex) ~= deliverStepSuccess then

        if quest:isExpired() then

            local talkBox = NPCTalkBox:new(width, height)
            talkBox:addString(3113) -- 그 긴급 환자는 이미 큰 병원으로 송환되었습니다.

            player:talk(npc, talkBox)

        else

            if player:hasInventoryItem(questItemId, 3) then

                local questChange = QuestChange:new()
                questChange:setFlag(deliverStepStateIndex, deliverStepSuccess)

                player:changeQuest(noviceQuestId, questChange)
                player:removeInventoryItem(questItemId, 3)

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
        local healingCost = 500

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

                player:recoverHP()
                player:recoverSP()

                if quest ~= nil and quest:getState() == 1 then

                    local talkBox = NPCTalkBox:new(width, height)
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
