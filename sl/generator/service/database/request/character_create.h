#pragma once
#include "sl/generator/service/database/request/item_create.h"
#include "sl/generator/service/database/request/skill_create.h"

namespace sunlight::req
{
    struct CharacterCreate
    {
        int64_t id = 0;
        int64_t aid = 0;
        int8_t sid = 0;
        int8_t slot = 0;
        std::string name;
        int32_t hair = 0;
        int32_t hairColor = 0;
        int32_t face = 0;
        int32_t skinColor = 0;
        int32_t zone = 0;
        float x = 0.f;
        float y = 0.f;

        int8_t gender = 0;
        int32_t str = 0;
        int32_t dex = 0;
        int32_t accr = 0;
        int32_t health = 0;
        int32_t intell = 0;
        int32_t wis = 0;
        int32_t will = 0;
        int8_t water = 0;
        int8_t fire = 0;
        int8_t lightning = 0;

        int32_t job = 0;

        std::vector<ItemCreate> items;
        std::vector<SkillCreate> skills;

        friend void to_json(nlohmann::json& j, const CharacterCreate& req);
    };
}
