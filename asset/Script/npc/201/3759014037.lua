-- Zone  : 201
-- Stage : 10014
-- Name  : 길드장 레니
-- Nes   : 100032

return function (system, npc, player, sequence)

    -- TODO: 겸직 로직 만들기

    local width = 400
    local height = 250

    local progressCheckQuestId = 1000

    if player:isFighter() then

        if player:isNovice() then

            local progressCheckQuest = player:findQuest(progressCheckQuestId)

            if progressCheckQuest ~= nil then

                local flag = progressCheckQuest:getFlag(1)

                if flag == 2 then -- 전직 담당에게서 퀘스트 받은 경우

                    local talkBox = NPCTalkBox:new(width, height)
                    talkBox:addString(1220) -- 흠, 전직시험을 열심히 보는 중이로군

                    player:talk(npc, talkBox)
                    player:disposeTalk()

                    return

                elseif flag == 3 then -- 성공 후 전직 담당과 얘기 끝

                    if sequence == 0 then

                        local talkBox = NPCTalkBox:new(width, height)
                        talkBox:addString(1230) -- 얘기는 전해들었지만서도, 꽤나 일을 잘 처리해서 맡긴 쪽에서도 아주 든든해하는 모양이더만

                        player:talk(npc, talkBox)

                    elseif sequence == 1 then

                        local talkBox = NPCTalkBox:new(width, height)
                        talkBox:addString(1231) -- 자, 그럼 앞으로 훌륭한 전사의 길을 가길 바라네

                        player:talk(npc, talkBox)
                        player:disposeTalk()

                        -- TODO: 전직 처리

                    end

                    return

                end

            end


            if sequence == 0 then

                local talkBox = NPCTalkBox:new(width, height)
                talkBox:addString(1130) -- 오~ 이것은 이것은 이것은 훌륭한 젊은이가 아닌가! 전사를 택하다니

                player:talk(npc, talkBox)

                return

            elseif sequence == 1 then

                local talkBox = NPCTalkBox:new(width, height)
                talkBox:addString(1131) -- 궁금한 점은 뭐든지 물어보게나.
                talkBox:addMenu(1132, 1133, 1) -- 직업에 대한 설명을 듣는다
                talkBox:addMenu(1134, 1135, 2) -- 전직에 대한 설명을 듣는다
                talkBox:addMenu(1136, 1137, 3) -- 전직신청을 한다
                talkBox:addMenu(1138, 1139, 4) -- 대화를 끝낸다

                player:talk(npc, talkBox)

                return

            elseif sequence == 2 then

                local selection = player:getSelection()

                player:setState(selection)

                if selection == 1 then -- 직업설명

                    local talkBox = NPCTalkBox:new(width, height)
                    talkBox:addString(1150) -- 전사라는 직업은 결코 쉬운 것이 아니지
                    talkBox:addMenu(1151, 1152, 1) -- 검투사에 대해 설명을 듣는다
                    talkBox:addMenu(1153, 1154, 2) -- 광전사에 대해 설명을 듣는다
                    talkBox:addMenu(1155, 1156, 3) -- 창기사에 대해 설명을 듣는다
                    talkBox:addMenu(1157, 1158, 4) -- 권법가에 대해 설명을 듣는다

                    player:talk(npc, talkBox)

                    return

                elseif selection == 2 then -- 전직설명

                    local talkBox = NPCTalkBox:new(width, height)
                    talkBox:addString(1170) -- 전직이라는 것은 더 높은 지위의 직업으로 올라간다는 걸 의미하지
                    talkBox:addString(1171) -- 시험은 언제든 전직담당에게 얘기한다면 중도에 포기할 수가 있네

                    player:talk(npc, talkBox)
                    player:setSequence(0) -- goto sequence 1

                    return

                elseif selection == 3 then -- 전직신청

                    if player:getNoviceJobLevel() < 20 then

                        local talkBox = NPCTalkBox:new(width, height)
                        talkBox:addString(1180) -- 아직 레벨 부족이라구 자네.<BR>최소한 <#FF4200>20레벨<#000000> 이상 되지 않으면 전직은 할 수 없네

                        player:talk(npc, talkBox)
                        player:setSequence(0) -- goto sequence 1

                        return

                    else

                        local talkBox = NPCTalkBox:new(width, height)
                        talkBox:addString(1190) -- 전직을 하고 싶다면 자신이 전직하고 싶은 직업의 담당자에게 가서 전직신청을 하도록 해. 친절하게 잘 도와줄테니. 어느 직업으로 할텐가?
                        talkBox:addMenu(1191, 1192, 1) -- 검투사로 전직한다
                        talkBox:addMenu(1193, 1194, 2) -- 광전사로 전직한다
                        talkBox:addMenu(1195, 1196, 3) -- 창기사로 전직한다
                        talkBox:addMenu(1197, 1198, 4) -- 권법가로 전직한다
                        talkBox:addMenu(1199, 1200, 999) -- 좀더 생각해 본 뒤 나중에 정한다

                        player:talk(npc, talkBox)

                        return

                    end

                elseif selection == 4 then -- 대화 끝내기

                    local talkBox = NPCTalkBox:new(width, height)
                    talkBox:addString(1210) -- 잘 가게나. 전사 길드는 전사 모두를 위한 장소이니 부담갖지 말고 자주 들르도록 하고.

                    player:talk(npc, talkBox)

                    return

                end

            elseif sequence == 3 then

                local prevSelection = player:getState()
                local selection = player:getSelection()

                if prevSelection == 1 then -- 직업 설명

                    local talkBox = NPCTalkBox:new(width, height)
                    talkBox:addString(1158 + selection)

                    player:talk(npc, talkBox)
                    player:setSequence(0) -- goto sequence 1

                    return

                elseif prevSelection == 3 then -- 전직신청

                    if selection == 999 then

                        -- 나중에 신청하겠다는 경우
                        local talkBox = NPCTalkBox:new(width, height)
                        talkBox:addString(1201)

                        player:talk(npc, talkBox)
                        player:disposeTalk()

                        return

                    end

                    local jobId = -1

                    if selection == 1 then

                        jobId = 2100

                    elseif selection == 2 then

                        jobId = 2101

                    elseif selection == 3 then

                        jobId = 2102

                    elseif selection == 4 then

                        jobId = 2103

                    end

                    local quest = Quest:new(progressCheckQuestId)
                    quest:setState(3)
                    quest:setFlag(0, jobId)
                    quest:setFlag(1, 1)

                    player:startQuest(quest)

                end

            end

        else

            local talkBox = NPCTalkBox:new(width, height)
            talkBox:addString(1120) -- 어떤가 전사의 길은?

            player:talk(npc, talkBox)

        end

    else

        local talkBox = NPCTalkBox:new(width, height)
        talkBox:addString(1110) -- 뭐야? 이 허약해빠진 녀석은? 예가 어디라고

        player:talk(npc, talkBox)

    end

    player:disposeTalk()
    
end

