#pragma once
#include "sl/data/sox/sox_column.h"

namespace sunlight
{
    struct SoxFile
    {
        std::string name;
        int32_t type;
        std::vector<SoxColumn> columnNames;
        int32_t rowCount;
        std::vector<char> gameData;

        static constexpr int64_t TYPE_OFFSET = 0x200C;
        static constexpr int64_t DATA_OFFSET = 0x220C;
        static constexpr int64_t MAX_COLUMN_BYTE_SIZE = 64;

        static auto CreateFrom(const std::filesystem::path& filePath) -> SoxFile;
    };
}
