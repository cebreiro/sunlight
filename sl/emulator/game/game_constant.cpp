#include "game_constant.h"

namespace sunlight
{
    // client calculates mix skill's level up required exp, but I don't understand that logic
    // to solve the problem, simply dump client's memory and use it
    // see client 0x462AEE
    const std::array<int32_t, GameConstant::MAX_MIX_SKILL_LEVEL> GameConstant::MIX_SKILL_LEVEL_UP_REQUIRED_EXP_D1 = {
        2200, 4653, 7365, 10342, 13590, 17350, 21396, 25734, 30614, 33578, 36828, 40370, 44210, 48354, 52808, 57578, 62938, 68629, 74657, 81305, 88305, 95663, 103671, 109451, 115583, 122073, 128927, 136452, 166852, 100
    };

    const std::array<int32_t, GameConstant::MAX_MIX_SKILL_LEVEL> GameConstant::MIX_SKILL_LEVEL_UP_REQUIRED_EXP_D2 = {
        3520, 7088, 10930, 15052, 19460, 24160, 29158, 34460, 39828, 45262, 50762, 56581, 62725, 69200, 76012, 83167, 90939, 99069, 107563, 116427, 125667, 135289, 145585, 156278, 167374, 178879, 191097, 203739, 252379, 100
    };

    const std::array<int32_t, GameConstant::MAX_MIX_SKILL_LEVEL> GameConstant::MIX_SKILL_LEVEL_UP_REQUIRED_EXP_D3 = {
        9460, 19272, 29668, 40431, 51799, 63549, 75925, 88698, 102118, 116197, 130697, 145877, 161749, 178325, 64283, 213110, 231602, 250843, 270845, 291620, 313180, 335537, 358989, 383843, 409539, 436089, 463505, 492100, 583300, 120
    };
}
