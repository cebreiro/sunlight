return function (player)

    function writeGlobalMessageHead (writer, entity)
        writer:writeUInt8(64)
        writer:writeUInt8(6)
        writer:writeUInt32(0)
        writer:writeInt64(entity:getId(), entity:getTypeValue())

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

    function sendTest4 (player)

        local writer = SlPacketWriter:new()

        writeGlobalMessageHead(writer, player)

        writer:writeInt32(5000)
        writer:writeInt32(5031) -- change skill mix exp
        writer:writeInt32(1301) 
        writer:writeInt32(166852)

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

    function writeMonsterAttackState (writer, player)

        local objectWriter = PacketWriter:new()

        objectWriter:writeUInt8(0xFF)
        objectWriter:writeUInt8(0x0F)
        objectWriter:writeUInt8(2) -- state_type normal attack
        objectWriter:writeUInt8(2) -- move type
        objectWriter:writeUInt8(0) -- unk4  --> client 0x495ECA 에 이 값이 그대로 나옴. 이후 1씩 증가. 범위 case [0, 3], sequence 인듯
        objectWriter:writeUInt8(0xFF) -- unk5

        objectWriter:writeFloat(0) -- destPosX
        objectWriter:writeFloat(0) -- destPosY
        objectWriter:writeFloat(0) -- destPosZ

        objectWriter:writeInt32(player:getId()) -- targetId
        objectWriter:writeInt32(player:getTypeValue()) -- targetType

        objectWriter:writeInt32(2) -- attackId
        objectWriter:writeInt32(0) -- motionId
        objectWriter:writeInt32(0) -- fxId
        objectWriter:writeInt32(0) -- param1
        objectWriter:writeInt32(0) -- param2
        objectWriter:writeInt32(1) -- 0: normalAttack, 1: skill1, 2: skill2 -> 다른 Animation 재생을 위해 필요

        writer:writeObject(objectWriter)

    end

    function writeMonsterSkillState (writer, player)

        local objectWriter = PacketWriter:new()

        objectWriter:writeUInt8(0xFF)
        objectWriter:writeUInt8(0x0F)
        objectWriter:writeUInt8(7) -- state_type normal attack
        objectWriter:writeUInt8(0) -- move type
        objectWriter:writeUInt8(0) -- unk4  --> client 0x495ECA 에 이 값이 그대로 나옴. 이후 1씩 증가. 범위 case [0, 4]
        objectWriter:writeUInt8(0xFF) -- unk5

        objectWriter:writeFloat(0) -- destPosX
        objectWriter:writeFloat(0) -- destPosY
        objectWriter:writeFloat(0) -- destPosZ

        objectWriter:writeInt32(player:getId()) -- targetId
        objectWriter:writeInt32(player:getTypeValue()) -- targetType

        objectWriter:writeInt32(2) -- attackId
        objectWriter:writeInt32(0xAA) -- motionId
        objectWriter:writeInt32(0xBB) -- fxId
        objectWriter:writeInt32(0xCC) -- param1
        objectWriter:writeInt32(1) -- param2  -> skill 사용할 땐 얘가 index 네
        objectWriter:writeInt32(140171) -- skillId

        writer:writeObject(objectWriter)

    end

    function SendMosnterAttackState (player)

        local monster = player:findNearestMonster()
        if monster ~= nil then

            local writer = SlPacketWriter:new()

            writeGlobalMessageHead(writer, monster)

            writer:writeInt32(0x71)
            writeMonsterAttackState(writer, player)

            player:send(writer)

        end
    end

    function SendMosnterSkillState (player)
        local monster = player:findNearestMonster()
        if monster ~= nil then

            local writer = SlPacketWriter:new()

            writeGlobalMessageHead(writer, monster)

            writer:writeInt32(0x71)
            writeMonsterSkillState(writer, player)

            player:send(writer)

        end
    end

    function sendStatusEffect (player)

        local writer = SlPacketWriter:new()

        writeGlobalMessageHead(writer, player)

        writer:writeInt32(2000) -- CHARSTATUSMSG
        writer:writeInt32(800)  -- CONDITION_STATE
        writer:writeInt32(1301) 
        writer:writeInt32(29)
        writer:writeInt32(0)

        player:send(writer)
    end

    function sendMonsterAttackResult (player)

        local monster = player:findNearestMonster()

        if monster ~= nil then

            local writer = SlPacketWriter:new()

            writeGlobalMessageHead(writer, monster)

            writer:writeInt32(126)

            local objectWriter = PacketWriter:new()

            objectWriter:writeInt32(player:getId()) -- targetId
            objectWriter:writeInt32(player:getTypeValue()) -- targetType
            objectWriter:writeInt32(76) -- id
            objectWriter:writeInt32(3) -- motionId (newEntityStateMotionId)
            objectWriter:writeInt32(0) --

            objectWriter:writeInt32(0) -- attackBlowType a[12]??
            objectWriter:writeInt32(104) -- a[11] -- maybe skillId
            objectWriter:writeInt32(1234) -- damage
            objectWriter:writeInt32(1700) -- a[10] --> weapon_class 네
            objectWriter:writeInt32(1) -- damgae_type
            objectWriter:writeInt32(1) -- damage count
            objectWriter:writeInt32(0) -- damage interval    a[5]
            objectWriter:writeInt32(0) -- attackBlowGroupId  a[14]
            objectWriter:writeInt32(0) -- attackedResultType

            writer:writeObject(objectWriter)

            player:send(writer)

        end
    end

        function writeMonsterAttackedState (writer, player)

        local objectWriter = PacketWriter:new()

        objectWriter:writeUInt8(0xFF)
        objectWriter:writeUInt8(0x0F)
        objectWriter:writeUInt8(0) -- state_type normal attack
        objectWriter:writeUInt8(0) -- move type
        objectWriter:writeUInt8(0) -- unk4  --> client 0x495ECA 에 이 값이 그대로 나옴. 이후 1씩 증가. 범위 case [0, 4]
        objectWriter:writeUInt8(0xFF) -- unk5

        objectWriter:writeFloat(0) -- destPosX
        objectWriter:writeFloat(0) -- destPosY
        objectWriter:writeFloat(0) -- destPosZ

        objectWriter:writeInt32(player:getId()) -- targetId
        objectWriter:writeInt32(player:getTypeValue()) -- targetType

        objectWriter:writeInt32(609) -- attackId
        objectWriter:writeInt32(3) -- motionId
        objectWriter:writeInt32(0) -- fxId
        objectWriter:writeInt32(0) -- param1
        objectWriter:writeInt32(1700) -- param2  -> skill 사용할 땐 얘가 index 네
        objectWriter:writeInt32(104) -- skillId

        writer:writeObject(objectWriter)

    end

    function SendMosnterAttackedState (player)

        local monster = player:findNearestMonster()
        if monster ~= nil then

            local writer = SlPacketWriter:new()

            writeGlobalMessageHead(writer, monster)

            writer:writeInt32(0x71)
            writeMonsterAttackedState(writer, player)

            player:send(writer)

        end
    end

    sendMonsterAttackResult(player)
    SendMosnterAttackedState(player)
    --sendGuildDESTROY(player)
    
end
