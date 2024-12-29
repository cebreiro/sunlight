#include "sox_file.h"

#include "sl/data/file_util.h"

namespace sunlight
{
    auto SoxFile::CreateFrom(const std::filesystem::path& filePath) -> SoxFile
    {
        const std::vector<char> fileData = ReadBinaryFile(filePath);
        const int64_t fileSize = std::ssize(fileData);

        StreamReader reader(fileData.begin(), fileData.end());

        SoxFile soxFile;

        soxFile.name = filePath.filename().stem().string();
        soxFile.type = reader.Read<int32_t>();

        const int32_t columnCount = reader.Read<int32_t>();
        soxFile.rowCount = reader.Read<int32_t>();

        soxFile.columnNames.resize(columnCount + 1);
        soxFile.columnNames[0] = SoxColumn{ SoxValueType::Int, "_index" };

        for (int32_t i = 0; i < columnCount; ++i)
        {
            std::array<char, MAX_COLUMN_BYTE_SIZE> columnName = {};

            reader.ReadBuffer(columnName.data(), std::ssize(columnName));

            soxFile.columnNames[i + 1].name = std::string(columnName.data());
        }

        reader.SetOffset(TYPE_OFFSET);
        for (int32_t i = 0; i < columnCount; ++i)
        {
            soxFile.columnNames[i + 1].type = ToSoxValueTypeFrom(reader.Read<int32_t>());
        }

        reader.SetOffset(DATA_OFFSET);
        soxFile.gameData.reserve(fileSize - reader.GetReadSize());

        std::ranges::copy(fileData.begin() + reader.GetReadSize(), fileData.end(),
            std::back_inserter(soxFile.gameData));

        for (int32_t i = 0; i < soxFile.rowCount; ++i)
        {
            for (const SoxColumn& column : soxFile.columnNames)
            {
                switch (column.type)
                {
                case SoxValueType::String:
                    reader.Skip(reader.Read<uint16_t>());
                    break;
                case SoxValueType::Int:
                case SoxValueType::Float:
                    reader.Skip(4);
                    break;
                case SoxValueType::None:
                default:
                    throw std::runtime_error(fmt::format("unhandled sox bytes type: {}, file: {}",
                        ToString(column.type), soxFile.name));
                }
            }
        }

        const std::string& end = reader.ReadString(5);
        if (::strcmp(end.c_str(), "THEND") != 0)
        {
            throw std::runtime_error(fmt::format("{} parsing fail, sox file: {}", __FUNCTION__, soxFile.name));
        }

        return soxFile;
    }
}
