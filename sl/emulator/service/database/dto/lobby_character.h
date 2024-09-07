#pragma once

namespace sunlight::db::dto
{
    struct LobbyCharacter
    {
        int64_t cid = 0;
        int8_t slot = 0;
        std::string name;
        int32_t level = 0;
        int8_t gender = 0;
        int32_t hair = 0;
        int32_t hairColor = 0;
        int32_t face = 0;
        int32_t skinColor = 0;
        int32_t zone = 0;
        int8_t arms = 0;

        int32_t hatItemId = 0;
        int32_t jacketItemId = 0;
        int32_t glovesItemId = 0;
        int32_t pantsItemId = 0;
        int32_t shoesItemId = 0;

        int32_t job1 = 0;
        int32_t job2 = 0;
    };
}
