#pragma once

namespace sunlight
{
    struct PartyInformation
    {
        std::string partyName;

        std::string leaderName;
        int8_t leaderCharacterLevel = 0;
        int32_t leaderZoneId = 0;

        int8_t memberCount = 0;
        bool isPublic = false;
        bool goldDistribution = false;
        bool itemDistribution = false;
    };
}
