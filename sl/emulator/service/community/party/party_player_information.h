#pragma once

namespace sunlight
{
    struct PartyPlayerInformation
    {
        std::string name;
        int32_t jobId = 0;
        int8_t jobLevel = 0;
        int32_t hp = 0;
        int32_t maxHP = 0;

        int64_t cid = 0;
    };
}
