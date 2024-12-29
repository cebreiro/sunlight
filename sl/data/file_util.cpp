#include "file_util.h"

namespace sunlight
{
    auto GetFileSize(std::ifstream& ifs) -> int64_t
    {
        const size_t current = ifs.tellg();

        ifs.seekg(0, std::ios::end);
        const size_t fileSize = ifs.tellg();

        ifs.seekg(current, std::ios::beg);

        return fileSize;
    }

    auto ReadBinaryFile(const std::filesystem::path& filePath) -> std::vector<char>
    {
        std::ifstream ifs(filePath, std::ios::binary);
        if (!ifs.is_open())
        {
            throw std::runtime_error(fmt::format("file: {} open fail", filePath.string()));
        }

        const int64_t fileSize = GetFileSize(ifs);

        std::vector<char> fileBuffer(fileSize);

        ifs.read(fileBuffer.data(), fileSize);
        ifs.close();

        return fileBuffer;
    }
}
