-- Zone  : 202
-- Stage : 10022
-- Name  : 칼빈슨 (무기점)
-- Nes   : 100093
-- Role  : 창기사, 광전사 전직 퀘스트, 트레저헌터 전직 퀘스트, 녹슨 단검 퀘스트

return function (system, npc, player, sequence)

    local width = 400
    local height = 250
    local stringBase = 10300

    local list = {}
    table.insert(list, player:findQuest(1002))
    table.insert(list, player:findQuest(1003))

    for i, jobChangeQuest in ipairs(list) do

        local step = jobChangeQuest:getFlag(1)

        if step > 2 and step < 5 then

            local talkBox = NPCTalkBox:new(width, height)
            talkBox:addString(stringBase + 10) -- <#FF4200>보라색 머리의 소녀<#000000>가 습격당하는 장면 말이지?
            talkBox:addString(stringBase + 11) -- 실은 이미 그 소녀가 무시무시한 파워로 녀석들에게 본때를 보여준 뒤였어. 나 같은 건 나설 곳도 없었다구.
            talkBox:addString(stringBase + 12) -- <#FF4200>버려진 연구소<#000000>에 가서 녀석들을 퇴치하면 복수는 이루어지는 게 아닐까?

            player:talk(npc, talkBox)

            if step ~= 4 then

                local questChange = QuestChange:new()
                questChange:setFlag(1, 4)

                player:changeQuest(jobChangeQuest:getId(), questChange)

            end

            player:disposeTalk()

            return

        end

    end

    -- TODO: 트레저헌터 전직 퀘스트


    -- TODO: 녹슨 단검 퀘스트

    local talkBox = NPCTalkBox:new(width, height)
    talkBox:addString(stringBase + 1) -- 여어, 어서 오시게나. 모험가로구만?
    talkBox:addString(stringBase + 2)

    player:talk(npc, talkBox)

    player:disposeTalk()

end
