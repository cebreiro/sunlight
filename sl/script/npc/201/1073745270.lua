return function (system, npc, player, sequence)

    local width = 400
    local height = 250

    local swordWarriorQuestId = 1001
    local swordWarriorQuest = player:findQuest(swordWarriorQuestId)

    local function processJobChangeSwordWarrior()

        local requiredItemId = swordWarriorQuest:getFlag(5)
        local step = swordWarriorQuest:getFlag(1)

        if step == 2 then -- 전직 퀘스트를 받고 처음

            if sequence == 0 then

                local talkBox = NPCTalkBox:new(width, height)
                talkBox:addString(10520) -- 뭐야, 당신이 전사길드에서 보낸 사람? 어쩐지 아직 초짜 티를 다 벗지 못한 것 같은데, 믿어도 되는 거에요?
                talkBox:addString(10521) -- 뭐 좋아. 일단 길드에서 보냈다고 하니 눈 딱 감고 믿어 보겠어요.

                player:talk(npc, talkBox)

                return

            elseif sequence == 1 then

                local talkBox = NPCTalkBox:new(width, height)
                talkBox:addString(10522) -- 그러니까 당신이 누가 그 설계도를 훔쳐갔는지 알아내줬으면 좋겠어요.
                talkBox:addMenu(10523, 10524, 1) -- 설계도에 대해 물어본다
                talkBox:addMenu(10525, 10526, 2) -- 의뢰를 수행하기 위해 떠난다

                player:talk(npc, talkBox)

                return

            elseif sequence == 2 then

                local selection = player:getSelection()

                if selection == 1 then

                    local talkBox = NPCTalkBox:new(width, height)
                    talkBox:addString(10527) -- 당신에게만 살짝 알려주는 거지만, <#08BB00>‘그림자 5호’<#000000>라고 해요.<BR>...뭐지?
                    talkBox:addString(10528) -- 뿐만 아니라 놀라지 말아요!

                    player:talk(npc, talkBox)
                    player:setSequence(0) -- goto sequence 1

                    return

                elseif selection == 2 then

                    local questChange = QuestChange:new()
                    questChange:setFlag(1, 3)

                    player:changeQuest(swordWarriorQuestId, questChange)

                end

            end

        elseif step >= 3 and step <= 996 then

            if player:hasInventoryItem(requiredItemId, 1) then

                if sequence == 0 then

                    local talkBox = NPCTalkBox:new(width, height)
                    talkBox:addString(10540) -- 흥, 수고했으니 좋은 것을 하나만 가르쳐 주죠. 그 설계도를 만든 사람이 누구인지...
                    talkBox:addString(10541) -- 생각보다 실력이 좋은 모양이군요, 당신. 길드에는 의뢰를 잘 완수했다고 알려줄 테니, 가서 전직과정을 마치면 되겠네요.

                    player:talk(npc, talkBox)

                    return

                elseif sequence == 1 then

                    player:removeInventoryItemAll(requiredItemId)

                    local questChange = QuestChange:new()
                    questChange:setFlag(1, 997)
                    questChange:setState(1)

                    player:changeQuest(swordWarriorQuestId, questChange)

                end

            else

                local talkBox = NPCTalkBox:new(width, height)
                talkBox:addString(10530) -- 아니, 아직도 <#FF4200>설계도<#000000>의 행방을 못 찾았어요? 둔해! 느려!

                player:talk(npc, talkBox)

            end

        elseif step == 997 then

            local talkBox = NPCTalkBox:new(width, height)
            talkBox:addString(10550) -- 흠, 아직도 내게 볼 일이 있는 건가요? 이미 의뢰는 완료했다고 전직담당에게 말을 해 두었는데.

            player:talk(npc, talkBox)

        end


        player:disposeTalk()

    end

    if swordWarriorQuest ~= nil then

        local state = swordWarriorQuest:getFlag(1)

        if state > 1 and state ~= 999 then

            processJobChangeSwordWarrior()

            return

        end

    end

    -- 퀘스트와 무관한 경우, 인삿말
    local talkBox = NPCTalkBox:new(width, height)
    talkBox:addString(10510) -- 나는 기드. 지금은 이렇게 허접하기 짝이 없는
    talkBox:addString(10511) -- 어디, 당신은 어떨까요? 지금 봐서는 별로 대단한 인물이 될 관상은 아닌 것 같지만서도오~

    player:talk(npc, talkBox)

    player:disposeTalk()

end
