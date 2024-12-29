
return function (system, npc, player, sequence)

    local noviceQuestId = 200
    local width = 400
    local height = 150

    local questStepflagIndex = 0

    local stepStart = 5
    local stepAchieveJobLevel = 10
    local stepCollectitem = 15
    local stepDeliver = 20
    local stepHunting = 25

    local deliverStepStateIndex = 4
    local deliverStepElapsedTime = 3

    local deliverStepStartState = 0
    local deliverStepSuccess = 10
    
    local quest = player:findQuest(noviceQuestId)

    if quest ~= nil then

        local state = quest:getState()
        local step = quest:getFlag(questStepflagIndex)

        if state == 1 then

            local talkBox = NPCTalkBox:new(width, height)
            talkBox:addString(3057) -- 모든 과정을 수료했으니 더 이상 이곳에 오래 머무르지 않는 것이 좋을 걸세

            player:talk(npc, talkBox)

        else
            if step == stepStart then

                if sequence == 0 then

                    local talkBox = NPCTalkBox:new(width, height + 25)
                    talkBox:addString(3010) -- 좋습니다. 정식으로 신청을 하셨으니 이제 당신은 초보자 캠프의 '정식 수련원'이 되셨습니다.
                    talkBox:addMenu(3012, 3013, 1) -- 이동 방법에 대해 질문한다
                    talkBox:addMenu(3014, 3015, 2) -- 전투 방법에 대해 질문한다
                    talkBox:addMenu(3016, 3017, 3) -- 스킬 사용 방법에 대해 질문한다
                    talkBox:addMenu(3018, 3019, 4) -- 능력치(스탯) 분배에 대해 질문한다
                    talkBox:addMenu(3020, 3021, 5) -- 여기에 있는 것 말고도 궁금한 것이 너무 많아
                    talkBox:addMenu(3022, 3023, 6) -- 임무 수행을 시작한다

                    player:talk(npc, talkBox)

                    return

                elseif sequence == 1 then

                    local selection = player:getSelection()

                    local talkBox = NPCTalkBox:new(width, height)
                    talkBox:addString(3024 + selection)

                    player:talk(npc, talkBox)

                    if selection == 6 then

                        local questChange = QuestChange:new()
                        questChange:setFlag(questStepflagIndex, stepAchieveJobLevel)

                        player:changeQuest(noviceQuestId, questChange)

                    end

                end

            elseif step == stepAchieveJobLevel then

                if player:getNoviceJobLevel() < 3 then

                    local talkBox = NPCTalkBox:new(width, height)
                    talkBox:addString(3031) -- 이봐 아직 직업레벨 3이 되지 못했잖아...

                    player:talk(npc, talkBox)

                else

                    if sequence == 0 then

                        local talkBox = NPCTalkBox:new(width, height)
                        talkBox:addString(3032) -- 흠, 직업레벨을 3까지 올려왔군.

                        player:talk(npc, talkBox)

                        return

                    elseif sequence == 1 then

                        local questChange = QuestChange:new()
                        questChange:setFlag(questStepflagIndex, stepCollectitem)

                        player:changeQuest(noviceQuestId, questChange)
                        player:addItem(4030002, 5)

                        local talkBox = NPCTalkBox:new(width, height)
                        talkBox:addString(3033) -- 이젠 제대로 된 방어구 하나쯤은 있어야

                        player:talk(npc, talkBox)

                    end

                end

            elseif step == stepCollectitem then

                local collectItemId = 5050017

                if player:hasInventoryItem(collectItemId, 5) then

                    if sequence == 0 then

                        local talkBox = NPCTalkBox:new(width, height)
                        talkBox:addString(3035) -- 흠... 이 정도면 유니폼 한 벌은 만들겠군

                        player:talk(npc, talkBox)

                        return

                    elseif sequence == 1 then

                        local questChange = QuestChange:new()
                        questChange:setFlag(deliverStepStateIndex, deliverStepStartState)
                        questChange:setFlag(deliverStepElapsedTime, 0)
                        questChange:setFlag(questStepflagIndex, stepDeliver)

                        local timeLimitMinutes = 15
                        questChange:configureTimeLimit(deliverStepElapsedTime, timeLimitMinutes)

                        player:changeQuest(noviceQuestId, questChange)
                        player:removeInventoryItem(collectItemId, 5)

                        if player:isMale() then
                            player:addItem(2091061, 1)
                            player:addItem(2091071, 1)
                        else
                            player:addItem(2591061, 1)
                            player:addItem(2591071, 1)
                        end

                        local talkBox = NPCTalkBox:new(width, height)
                        talkBox:addString(3036) -- 자, 이번에는 시간이 촉박한 임무다

                        player:talk(npc, talkBox)

                    end

                else

                    local talkBox = NPCTalkBox:new(width, height)
                    talkBox:addString(3034) -- 양털을 어디서 얻을 수 있냐고?

                    player:talk(npc, talkBox)

                end

            elseif step == stepDeliver then

                local deliverStepState = quest:getFlag(deliverStepStateIndex)

                function setQuestNextStep ()
                    local questChange = QuestChange:new()
                    questChange:setFlag(deliverStepStateIndex, -1)

                    player:changeQuest(noviceQuestId, questChange)
                end

                if deliverStepState == deliverStepStartState then

                    if quest:isExpired() then

                        setQuestNextStep()

                        local talkBox = NPCTalkBox:new(width, height)
                        talkBox:addString(3041) -- 이런... 제한 시간을 초과해버렸으므로 이번 임무는 실패다.

                        player:talk(npc, talkBox)

                    else

                        local talkBox = NPCTalkBox:new(width, height)
                        talkBox:addString(3037) -- 출장소는 맵의 중앙 윗 부분에 위치하고 있으니 미니맵을 참조하면 찾기 쉬울 것이다

                        player:talk(npc, talkBox)

                    end

                elseif deliverStepState == deliverStepSuccess then

                    if sequence == 0 then

                        local talkBox = NPCTalkBox:new(width, height)
                        talkBox:addString(3038) -- 모든 임무를 제한 시간 내에 해결해 왔으니 보상을 주지

                        player:talk(npc, talkBox)

                        return

                    elseif sequence == 1 then

                        -- TODO: impl reward
                        setQuestNextStep()

                        local talkBox = NPCTalkBox:new(width, height)
                        talkBox:addString(3039) -- 이번 임무를 계기로... 

                        player:talk(npc, talkBox)

                    end

                else
                    if sequence == 0 then
                        local talkBox = NPCTalkBox:new(width, height)
                        talkBox:addString(3043) -- 자, 이제 이 초보자 캠프의 코스도 슬슬 막바지에 접어들었다

                        player:talk(npc, talkBox)

                        return

                    elseif sequence == 1 then
                        local talkBox = NPCTalkBox:new(width, height)
                        talkBox:addString(3044) -- 파티에 대해서 아무것도 모른다고?
                        talkBox:addMenu(3045, 3046, 1) -- 파티 및 파티의 장점에 대해 질문한다
                        talkBox:addMenu(3047, 3048, 2) -- 파티하는 방법에 대해 질문한다
                        talkBox:addMenu(3049, 3050, 3) -- 임무를 수행하러 간다

                        player:talk(npc, talkBox)

                        return

                    elseif sequence == 2 then

                        local selection = player:getSelection()
                        local talkBox = NPCTalkBox:new(width, height)
                        talkBox:addString(3050 + selection)

                        player:talk(npc, talkBox)

                        if selection == 3 then

                            local questChange = QuestChange:new()
                            questChange:setFlag(questStepflagIndex, stepHunting)

                            player:changeQuest(noviceQuestId, questChange)

                        end

                    end

                end

            elseif step == stepHunting then

                local requiredItemId = 5050016

                if player:hasInventoryItem(requiredItemId, 1) then

                    if sequence == 0 then

                        local talkBox = NPCTalkBox:new(width, height)
                        talkBox:addString(3055) -- 과연, 슬라푸딩 프린스의 핵을 구해왔군.

                        player:talk(npc, talkBox)

                        return

                    elseif sequence == 1 then

                        local questChange = QuestChange:new()
                        questChange:setState(1)

                        player:changeQuest(noviceQuestId, questChange)
                        player:removeInventoryItem(requiredItemId, 1)

                        local talkBox = NPCTalkBox:new(width, height)
                        talkBox:addString(3056) -- 이것으로 초보자 캠프의 모든 과정은 끝이 났다. 루크레치아에 가면 엔테롤 대륙으로 가는 길을...

                        player:talk(npc, talkBox)

                    end

                else

                    local talkBox = NPCTalkBox:new(width, height)
                    talkBox:addString(3054) -- 슬라푸딩 프린스 퇴치는 초보자 캠프 최후의 관문인 만큼 만만하진 않을 거야.

                    player:talk(npc, talkBox)

                end

            end

        end

    else
        local acceptQuest = 1000
        local rejectQuest = 2000

        if player:getNoviceJobLevel() < 3 then

            if sequence == 0 then

                local talkBox = NPCTalkBox:new(width, height)
                talkBox:addString(3003) -- 안녕하세요? 세계에 오신 것을 환영합니다.
                talkBox:addString(3004) -- 지금 초보자 캠프 과정을 신청하겠습니까?
                talkBox:addMenu(3005, 3006, acceptQuest) -- 예, 지금 신청하겠습니다.
                talkBox:addMenu(3007, 3008, rejectQuest) -- 아니오, 나중에 신청하겠습니다.

                player:talk(npc, talkBox)

                return

            elseif sequence == 1 then

                local selection = player:getSelection()

                if selection == acceptQuest then

                    local quest = Quest:new(noviceQuestId)
                    quest:setFlag(questStepflagIndex, stepStart)

                    -- quest 생성 시, 이후 사용할 모든 flag 를 설정해야 나중에 수정이 가능함
                    quest:setFlag(deliverStepStateIndex, -1)
                    quest:setFlag(deliverStepElapsedTime, -1)

                    player:startQuest(quest)

                elseif selection == rejectQuest then

                    local talkBox = NPCTalkBox:new(width, height)
                    talkBox:addString(3009) -- 좋습니다. 언제든지 준비가 되었을 때 초보자 과정을 신청하세요

                    player:talk(npc, talkBox)

                end
            end
        else
            local talkBox = NPCTalkBox:new(width, height)
            talkBox:addString(3002) -- 당신은 이미 직업 레벨 3 이상이 되셨군요.

            player:talk(npc, talkBox)

        end

    end

    player:disposeTalk()

end
