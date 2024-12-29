
return function (system, npc, player, sequence)

    local width = 400
    local height = 250

    if sequence == 0 then

        local talkBox = NPCTalkBox:new(width, height)
        talkBox:addString(2501) -- 으음... 벌써 가려는 건가요?
        talkBox:addString(71) -- <BR>

        talkBox:addMenu(2502, 2503, 1) -- 벨슈로트
        talkBox:addMenu(2504, 2505, 2) -- 엔트란스
        talkBox:addMenu(2506, 2507, 3) -- 아무 데도 가지 않고 취소한다

        player:talk(npc, talkBox)

        return

    elseif sequence == 1 then

        local selection = player:getSelection()

        if selection == 1 or selection == 2 then

            player:setState(selection)

            local talkBox = NPCTalkBox:new(width, height)
            talkBox:addString(72) -- <B>
            talkBox:addString(104) -- <#3E1CBA>, //이름용 청색
            talkBox:addString(2500) -- 루크레치아
            talkBox:addString(101) -- <#000000> //검정
            talkBox:addString(73) -- </B>
            talkBox:addString(2510) -- 단, 한번 나가면 다시 돌아올수 없으니

            talkBox:addMenu(203, 204, 1) -- 확실하다
            talkBox:addMenu(201, 202, 2) -- 취소한다

            player:talk(npc, talkBox)

            return

        end

    elseif sequence == 2 then

        local selection = player:getSelection()

        if selection == 1 then

            local talkBox = NPCTalkBox:new(width, height)
            talkBox:addString(2508) --뭐, 잘가도록 하세요.

            player:talk(npc, talkBox)

            --  현재 초캠 퀘스트 중이라면 실패시킨다
            local quest = player:findQuest(200)
            if quest ~= nil and quest:getState() ~= 1 then

                local questChange = QuestChange:new()
                questChange:setState(2)

                player:changeQuest(200, questChange)

            end

            -- 슬라푸딩 핵 제거
            player:removeInventoryItemAll(5050016)

            return

        elseif selection == 2 then

            local talkBox = NPCTalkBox:new(width, height)
            talkBox:addString(2509) -- 좀 더 초보자 캠프를 둘러보고 싶으신 모양이군요.

            player:talk(npc, talkBox)

        end

    elseif sequence == 3 then

        local state = player:getState()

        if state == 1 then

            player:changeZone(201, 11560, 8800)

        elseif state == 2 then

            player:changeZone(202, 12240, 5750)

        end

    end

    player:disposeTalk()

end
