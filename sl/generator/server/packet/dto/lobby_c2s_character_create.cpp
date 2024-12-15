#include "lobby_c2s_character_create.h"

namespace sunlight
{
    auto LobbyC2SCharacterCreate::CreateFrom(BufferReader& reader) -> LobbyC2SCharacterCreate
    {
        LobbyC2SCharacterCreate result;

        result.name = reader.ReadString(32);
        result.gender = static_cast<int8_t>(reader.Read<int32_t>());
        result.str = reader.Read<int32_t>();
        result.dex = reader.Read<int32_t>();
        result.accr = reader.Read<int32_t>();
        result.health = reader.Read<int32_t>();
        result.intell = reader.Read<int32_t>();
        result.wis = reader.Read<int32_t>();
        result.will = reader.Read<int32_t>();
        result.water = static_cast<int8_t>(reader.Read<int32_t>());
        result.fire = static_cast<int8_t>(reader.Read<int32_t>());
        result.lightning = static_cast<int8_t>(reader.Read<int32_t>());
        result.unk01 = reader.Read<int32_t>();
        result.hairColor = reader.Read<int32_t>();
        result.skinColor = reader.Read<int32_t>();
        result.face = reader.Read<int32_t>();
        result.hair = reader.Read<int32_t>();
        result.jacketId = reader.Read<int32_t>();

        [[maybe_unused]]
        const int32_t glovesModelId = reader.Read<int32_t>();

        result.pantsId = reader.Read<int32_t>();
        result.shoesId = reader.Read<int32_t>();
        result.unk02 = reader.Read<int32_t>();
        result.unk03 = reader.Read<int32_t>();
        result.unk04 = reader.Read<int32_t>();
        result.unk05 = reader.Read<int32_t>();
        result.unk06 = reader.Read<int32_t>();
        result.unk07 = reader.Read<int32_t>();
        result.unk08 = reader.Read<int32_t>();
        result.unk09 = reader.Read<int32_t>();
        result.unk10 = reader.Read<int32_t>();
        result.unk11 = reader.Read<int32_t>();
        result.unk12 = reader.Read<int32_t>();
        result.unk13 = reader.Read<int32_t>();
        result.unk14 = reader.Read<int32_t>();
        result.unk15 = reader.Read<int32_t>();
        result.unk16 = reader.Read<int32_t>();
        result.unk17 = reader.Read<int32_t>();
        result.job1 = reader.Read<int32_t>();

        return result;
    }
}
