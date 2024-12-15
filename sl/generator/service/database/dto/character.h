#pragma once
#include "sl/generator/service/database/enum/item_pos_type.h"

namespace sunlight::db::dto
{
    struct Character
    {
        int64_t id = 0;
        int64_t aid = 0;
        std::string name;
        int32_t hair = 0;
        int32_t hairColor = 0;
        int32_t face = 0;
        int32_t skinColor = 0;
        int8_t arms = 0;
        int8_t running = 0;
        int8_t dead = 0;
        int32_t gold = 0;
        int8_t inventoryPage = 0;
        int32_t zone = 0;
        int32_t stage = 0;
        float x = 0.f;
        float y = 0.f;
        float yaw = 0.f;

        int8_t gender = 0;
        std::optional<int32_t> hp = std::nullopt;
        std::optional<int32_t> sp = std::nullopt;
        int32_t level = 0;
        int32_t exp = 0;
        int32_t statPoint = 0;
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

        struct Job
        {
            int32_t id = 0;
            int32_t type = 0;
            int32_t level = 0;
            int32_t exp = 0;
            int32_t skillPoint = 0;
        };
        std::vector<Job> jobs;

        struct Skill
        {
            int32_t id = 0;
            int32_t job = 0;
            int32_t level = 0;
            int32_t cooldown = 0;
            int8_t page = 0;
            int8_t x = 0;
            int8_t y = 0;
            int32_t exp = 0;
        };
        std::vector<Skill> skills;

        struct Item
        {
            int64_t id = 0;
            int32_t dataId = 0;
            int32_t quantity = 0;
            ItemPosType posType = {};
            int8_t page = 0;
            int8_t x = 0;
            int8_t y = 0;
        };
        std::vector<Item> items;

        struct Quest
        {
            int32_t id = 0;
            int32_t state = 0;
            std::string flags;
            std::string data;
        };
        std::vector<Quest> quests;

        struct ProfileSetting
        {
            int8_t refusePartyInvite = 0;
            int8_t refuseChannelInvite = 0;
            int8_t refuseGuildInvite = 0;
            int8_t privateProfile = 0;
        };

        ProfileSetting profileSetting = {};
    };
}
