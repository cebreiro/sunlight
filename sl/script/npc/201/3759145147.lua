return function (system, npc, player, sequence)

    local width = 400
    local height = 250
    local stringBase = 26000

    if not player:isArtisan() then

        local talkBox = NPCTalkBox:new(width, height)
        talkBox:addString(stringBase + 1) -- <B><#3E1CBA>조합사길드장 스칼렛<#000000><BR></B>
        talkBox:addString(stringBase + 2) -- 죄송합니다만 이곳은 조합사 길드. 다른 직업의 업무 처리는 하지 않고 있습니다. 자신의 직업에 맞는 길드를 찾아주세요.

        player:talk(npc, talkBox)
        player:disposeTalk()

    end

    local progressQuestId = 210
    local progressQuest = player:findQuest(progressQuestId)
    if progressQuest ~= nil then

        local step = progressQuest:getFlag(1)

        if step == 3 then

            local talkBox = NPCTalkBox:new(width, height)
            talkBox:addString(stringBase + 57) -- 시험은 잘 진행되고 있나요?

            player:talk(npc, talkBox)
            player:disposeTalk()

            return

        elseif step == 4 then

            local talkBox = NPCTalkBox:new(width, height)
            talkBox:addString(stringBase + 55) -- 음~ 어느새 늠름한 조합사의 모습을 갖추셨군요.  어때요?<BR> 

            player:talk(npc, talkBox)
            player:disposeTalk()

            return


        elseif step == 5 then

            if sequence == 0 then

                local talkBox = NPCTalkBox:new(width, height)
                talkBox:addString(stringBase + 53) --전직 담당으로부터 시험에 대한 보고서는 잘 받아 보았습니다

                player:talk(npc, talkBox)

                return

            elseif sequence == 1 then

                local talkBox = NPCTalkBox:new(width, height)
                talkBox:addString(stringBase + 54) -- 이제 당신은 초보 조합사가 아닌 전문 분야에서 인정 받은 전문 조합사가 되셨습니다


                player:talk(npc, talkBox)

                return

            elseif sequence == 2 then

                local advancedJobList = { 2414, 2412,  2413, 2415 }
                local jobChangeSelection = progressQuest:getFlag(0)

                local advancedJob = advancedJobList[jobChangeSelection] or -1

                if not player:promoteJob(advancedJob) then

                    system:error("fail to premote job change quest - job: " .. advancedJob)
                    player:disposeTalk()

                    return

                end

                local questChange = QuestChange:new()
                questChange:setFlag(1, 4)

                player:changeQuest(progressQuestId, questChange)

                local jobChangeQuestChange = QuestChange:new()
                jobChangeQuestChange:setState(1)

                player:changeQuest(200 + jobChangeSelection, jobChangeQuestChange)

                -- TODO: 인벤토리 1페이지 추가
                -- TODO: Notice (14002001, 인벤토리가 확장되었습니다.)

            end

            player:disposeTalk()

            return

        end

    end

    ::handle_state::

    local state = player:getState()

    if state == 0 then

        if sequence == 0 then

            local talkBox = NPCTalkBox:new(width, height)
            talkBox:addString(stringBase + 3) -- 조합사 길드에 오신 것을 환영합니다.
            talkBox:addMenu(stringBase + 4, stringBase + 5, 1) -- 직업에 대한 설명을 듣는다
            talkBox:addMenu(stringBase + 6, stringBase + 7, 2) -- 전직에 대한 설명을 듣는다
            talkBox:addMenu(stringBase + 8, stringBase + 9, 3) -- 전직 신청을 한다
            talkBox:addMenu(stringBase + 10, stringBase + 11, 4) -- 대화를 끝낸다

            player:talk(npc, talkBox)

            return

        elseif sequence == 1 then

            local selection = player:getSelection()
            player:setState(selection)

            goto handle_state

        end

    elseif state == 1 then -- 직업에 대한 설명을 듣는다

        if sequence == 1 then

            local talkBox = NPCTalkBox:new(width, height)
            talkBox:addString(stringBase + 14) -- 어떤 조합사에 대해 알려드릴까요?
            talkBox:addMenu(stringBase + 15, stringBase + 16, 1) -- 요리사에 대한 설명을 듣는다
            talkBox:addMenu(stringBase + 17, stringBase + 18, 2) -- 대장장이에 대한 설명을 듣는다
            talkBox:addMenu(stringBase + 19, stringBase + 20, 3) -- 디자이너에 대한 설명을 듣는다
            talkBox:addMenu(stringBase + 21, stringBase + 22, 4) -- 세공사에 대한 설명을 듣는다
            talkBox:addMenu(stringBase + 23, stringBase + 24, 5) -- 별로 궁금한게 없다

            player:talk(npc, talkBox)

            return

        elseif sequence == 2 then

            local selection = player:getSelection()

            if selection ~= 5 then

                local talkBox = NPCTalkBox:new(width, height)
                talkBox:addString(stringBase + 25 + selection)

                player:talk(npc, talkBox)
                player:setSequence(0) -- goto sequence 1

                return

            else

                player:setState(0)
                player:setSequence(0)

                sequence = 0

                goto handle_state

            end

        end

    elseif state == 2 then -- 전직에 대한 설명을 듣는다

        local talkBox = NPCTalkBox:new(width, height)
        talkBox:addString(stringBase + 31) -- 전직은 쉽게 말해 좀 더 상위 계열의 조합사가 되는 것을 뜻하죠

        player:talk(npc, talkBox)
        player:setSequence(-1) -- goto sequence 0
        player:setState(0)

        return

    elseif state == 3 then -- 전직 신청을 한다

        if player:getNoviceJobLevel() < 20 then

            local talkBox = NPCTalkBox:new(width, height)
            talkBox:addString(stringBase + 33) --  이런, 의지는 좋으나 아직 전직 시험을 치르기엔 레벨이 많이 부족해요

            player:talk(npc, talkBox)
            player:setSequence(-1) -- goto sequence 0
            player:setState(0)

            return

        end

        local passApproval = false

        if sequence == 1 and progressQuest ~= nil then

            local step = progressQuest:getFlag(1)
            if step == 1 then

                passApproval = true

                sequence = 2
                player:setSequence(sequence)

            end

        end

        if sequence == 1 then

            local talkBox = NPCTalkBox:new(width, height)
            talkBox:addString(stringBase + 34) --  그 동안 꽤나 열심히 수련을 했나보군요. 충분한 레벨 자격요건이 되시니 전직 시험 신청을 승인하도록 하죠

            player:talk(npc, talkBox)

            return

        elseif sequence == 2 then

            local talkBox = NPCTalkBox:new(width, height)

            if passApproval then

                talkBox:addString(stringBase + 36) -- 왜 그러세요?  아무래도 신청한 직업에 대한 생각이 바뀌 었나보군요.

            else

                talkBox:addString(stringBase + 35) -- 만약 진행 중 그만두고 싶거나 다른 직업으로 전환하고 싶다면 해당 전직 담당에게 이야기를 하면 됩니다.

            end

            talkBox:addMenu(stringBase + 39, stringBase + 40, 1) -- 요리사로 전직한다
            talkBox:addMenu(stringBase + 41, stringBase + 42, 2) -- 대장장이로 전직한다
            talkBox:addMenu(stringBase + 43, stringBase + 44, 3) -- 디자이너로 전직한다
            talkBox:addMenu(stringBase + 45, stringBase + 46, 4) -- 세공사로 전직한다
            talkBox:addMenu(stringBase + 47, stringBase + 48, 5) -- 좀 더 생각해 본 뒤 나중에 정한다

            player:talk(npc, talkBox)

            return

        elseif sequence == 3 then

            local selection = player:getSelection()

            if selection == 5 then

                local talkBox = NPCTalkBox:new(width, height)
                talkBox:addString(stringBase + 51) -- 자신이 앞으로 가야 할 길을 정한다는 것은 쉬운 일이 아니죠.

                player:talk(npc, talkBox)
                player:setSequence(-1) -- goto sequence 0
                player:setState(0)

                return

            end

            local talkBox = NPCTalkBox:new(width, height)
            talkBox:addString(stringBase + 50) -- 네, 승인 시험을 보기에 충분한 레벨에 도달했으니 시험 응시를 승인하도록 하겠습니다

            player:talk(npc, talkBox)

            if selection >= 1 and selection <= 4 then

                for i = 201, 204 do

                    if player:findQuest(i) ~= nil then

                        local questChange = QuestChange:new()
                        questChange:setState(3)

                        player:changeQuest(i, questChange)

                    end

                end

                if progressQuest ~= nil then

                    local questChange = QuestChange:new()
                    questChange:setFlag(0, selection)
                    questChange:setFlag(1, 1)

                    player:changeQuest(progressQuestId, questChange)

                else

                    local quest = Quest:new(progressQuestId)
                    quest:setState(3)
                    quest:setFlag(0, selection)
                    quest:setFlag(1, 1)

                    player:startQuest(quest)

                end

            end

        end


    elseif state == 4 then -- 대화를 끝낸다

        local talkBox = NPCTalkBox:new(width, height)
        talkBox:addString(stringBase + 12) -- 언제든지 궁금한 것이 있을 때는 저를 찾아오도록 해요.

        player:talk(npc, talkBox)

    end


    player:disposeTalk()

end
