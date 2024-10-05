return function (player)

    function writeGlobalMessageHead (writer, player)
        writer:writeUInt8(64)
        writer:writeUInt8(6)
        writer:writeUInt32(0)
        writer:writeInt64(player:getId(), player:getTypeValue())

    end

    function writeNormalMessageHead (writer)
        writer:writeUInt8(64)
        writer:writeUInt8(5)

    end

    function writeCharacterMessageHead (writer)
        writer:writeUInt8(15)
        writer:writeUInt8(7)
    end

    function sendStatePropotion (player)

        local writer = SlPacketWriter:new()

        writeGlobalMessageHead(writer, player)

        writer:writeInt32(134)
        writer:writeString('<#00A0F3>asdasdasd')
        writer:writeInt32(0) -- 3
        writer:writeInt32(6002) -- 4 -> type
        writer:writeInt32(1) -- 5    -> 0 이면 '노점상 판매 물건 정리 중'

        writer:writeInt8(0)

        --[[
        local objectWriter = PacketWriter:new()

        objectWriter:writeFixedSizeString('wher are you?', 128)-- 사진? 이미지?
        objectWriter:writeFixedSizeString('10', 128) -- ok
        objectWriter:writeFixedSizeString('sex', 128) -- ok
        objectWriter:writeFixedSizeString('ddddd', 128) -- 사진? 이미지?
        objectWriter:writeFixedSizeString('cebreiro@github.com', 128) -- ok
        objectWriter:writeFixedSizeString('aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa', 1024) -- ok

        writer:writeObject(objectWriter)
        --]]

        player:send(writer)
    end

    function sendStreetVender (player)

        local writer = SlPacketWriter:new()

        writeNormalMessageHead(writer)

        writer:writeInt32(704) -- ZMSG_GROUP_MSG
        writer:writeInt32(1601) -- ZMSG_GROUP_MSG
        writer:writeString('<#00A0F3>asdasdasd')
        writer:writeInt32(0) -- 3
        writer:writeInt32(6002) -- 4 -> type
        writer:writeInt32(1) -- 5    -> 0 이면 '노점상 판매 물건 정리 중'
        writer:writeInt8(0)

        player:send(writer)
    end

    function createOpenTradeGroup (player)

        local writer = SlPacketWriter:new()

        writeNormalMessageHead(writer)

        writer:writeInt32(701)
        writer:writeInt32(1) -- 1 ? 0 ?  이게 그룹 아이디인 것 같은데? GroupMessage 에서 이걸로 찾는듯?
        writer:writeInt32(5001) -- 

        player:send(writer)
    end

    function createOpenStreetVenderGroup (player)

        local writer = SlPacketWriter:new()

        writeNormalMessageHead(writer)

        writer:writeInt32(701)
        writer:writeInt32(1) -- 
        writer:writeInt32(5001) -- 

        player:send(writer)
    end

    function sendTest (player)

        local writer = SlPacketWriter:new()

        writeNormalMessageHead(writer)

        writer:writeInt32(704)
        -- 4, 5 : 서버에 player billing 을 보내는 어떤 그룹
        -- 701: 거래, 노점상, mix Skill 생성 -> 퉁쳐서 Group 이라고 하는듯
        -- 702: 거래, 노점상 등 생성 실패
        -- 703: 거래, 노점상 등 생성 실패
        -- 704: 그룹 메시지

        -- 706: 인테리어 그룹 전용, 그룹 파괴
        -- 723: 인테리어 체인지

        writer:writeInt32(100003) -- id 맞는듯 ㅇㅅㅇ
        writer:writeInt32(700)
        writer:writeInt32(player:getId())
        writer:writeInt32(player:getTypeValue())
        writer:writeInt32(1011)
        writer:writeInt32(123)
        writer:writeString('aaaaaaa')
        writer:writeString('aaaaaaa')
        writer:writeString('bbbbbb')
        -- 700: Trade 연결??
            -- 먼저 개설하고 있었던 쪽에 이거 보내주면 characterName 나옴
        -- 800: Trade 관련
            -- 트레이드 창 닫힘??
        -- 900: Trade
            -- 1003 상대가 올려놓은 골드 변경
            -- 1005 상대편 confirm
            -- 1006, 1007 올려놓은 아이템이 안 보임(마우스 커서 올리면 정보는 나옴)
            -- 1010, 1011 올려놓은 아이템이 나타남. 그러나 pick item 이 사라지지않음 -> 락이 안 풀려서 다시 쏘면 클리어됨
        -- 900: StreetVendor
            -- 1032 아이템을 노점상에 올렷을 때 클라가 서버로 보냄, 안 올려도 그냥 클릭만 해도 보내는 것 같은데? -> 그냥 클릭만하면 보냄
            -- 1306, 노점상 시작 요청
            -- 1038: 구입 결과
            -- 1309, 1310, 서버에서 보내면 앉는 모션 나옴
        -- 1201: 방장만이 다른 참가자를 퇴장시킬 수 있습니다.
        -- 1302: "%s" 님은 현재 "%s" 에 참가하지 않아 영구 퇴장시킬 수 없습니다.
        -- 1303: "%s" 님을 영구 퇴장시켰습니다.
        -- 3200: "%s" 제목이 "%s" 로 바뀌었습니다. -> 로비
        -- 3201: 방장만이 제목을 바꿀 수 있습니다.
        -- 3300: 1: "%s" 공개로 바뀌었습니다. 2: "%s" 비공개로 바뀌었습니다.
        -- 3301: 방장만이 상태를 바꿀 수 있습니다.
        -- 3400: "%s" 비밀번호가 "%s" 로 바뀌었습니다. -> 로비
        -- 3401: 방장만이 비밀번호를 바꿀 수 있습니다.
        -- 3500:"%s" 최대 참가자수가 %d명으로 바뀌었습니다. -> 로비
        -- 3501: 방장만이 최대 참가자 수를 바꿀 수 있습니다.

        player:broadcast(writer, true)
    end

    function sendTest2 (player)

        local writer = SlPacketWriter:new()
        writeNormalMessageHead(writer)

        writer:writeInt32(704)
        writer:writeInt32(1) -- id 맞는듯 ㅇㅅㅇ
        writer:writeInt32(900)
        writer:writeInt32(1310)
        writer:writeInt32(0)
        writer:writeString('aaaaaaaaa')

        writer:writeInt32(1310) -- 
        writer:writeInt32(10) --  -2   operation type, 1이 가격 설정?
        writer:writeInt32(1) --  -3
        writer:writeInt32(0) --  -4   index
        writer:writeInt32(333441) --  -5  item 가격
        writer:writeInt32(0)
        writer:writeInt32(0)
        writer:writeInt32(0)
        writer:writeInt32(1234)

        player:send(writer)

    end

    function sendTest3 (player)

        local writer = SlPacketWriter:new()

        writeGlobalMessageHead(writer, player)

        writer:writeInt32(5000)
        writer:writeInt32(142) 
        writer:writeInt32(1301) 
        writer:writeInt32(900) 
        writer:writeInt32(50) 

        player:send(writer)
    end

    function sendTest4 (player)

        local writer = SlPacketWriter:new()

        writeGlobalMessageHead(writer, player)

        writer:writeInt32(5000)
        writer:writeInt32(5031) -- change skill mix exp
        writer:writeInt32(1301) 
        writer:writeInt32(166852)

        player:send(writer)
    end

    function sendTest5 (player)

        local writer = SlPacketWriter:new()

        writeGlobalMessageHead(writer, player)

        writer:writeInt32(5000)
        writer:writeInt32(5007) 
        writer:writeInt32(1) 
        writer:writeInt32(2)
        writer:writeInt32(0)

        player:send(writer)
    end

    function sendTest6 (player)

        local writer = SlPacketWriter:new()

        writeGlobalMessageHead(writer, player)

        writer:writeInt32(5000)
        writer:writeInt32(5020)  -- SLV2_SKILL_LEVEL_CHANGE
        writer:writeInt32(1301) 
        writer:writeInt32(29)
        writer:writeInt32(0)

        player:send(writer)
    end

    --createOpenStreetVenderGroup(player)
    --sendTest2(player)
    --send1705(player)

    sendTest4(player)
    --sendTest4(player)
    
end
