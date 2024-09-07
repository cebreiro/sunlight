#pragma once

namespace sunlight
{
    // search
	// push : 68
	// value: XX 70 00 00
	ENUM_CLASS(LobbyPacketC2S, int32_t,
        (ClientVersion, 0x7000) // sub_5961B0
        (Authentication, 0x7002) // sub_596230
		(CharacterList, 0x7004)
		(CharacterSelect, 0x7005)
		(CharacterCreate, 0x7007)
		(CharacterDelete, 0x7008)
		(CharacterNameCheck, 0x700A)
		(UnknownOpcode0x7040, 0x7040) // sub_596320
	)
}
