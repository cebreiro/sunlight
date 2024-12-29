#pragma once

namespace sunlight::db::dto
{
    struct Account
    {
        int64_t id = 0;
        std::string account;
        std::string password;
        int8_t gmLevel = 0;
        int8_t banned = 0;
        int8_t deleted = 0;
    };
}
