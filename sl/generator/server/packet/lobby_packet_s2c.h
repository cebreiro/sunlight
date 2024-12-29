#pragma once

namespace sunlight
{
    ENUM_CLASS(LobbyPacketS2C, int32_t,
        (ClientVersion, 0x8000)
        (Authentication, 0x8002)
        (CharacterList, 0x8003)
        (CharacterSelect, 0x8004)
        (CharacterCreate, 0x8006)
        (CharacterDelete, 0x8007)
        (CharacterNameCheck, 0x8011)
        (EmptyOperation0x8025, 0x8025)
        (EmptyOperation0x8026, 0x8026)
    )
}

