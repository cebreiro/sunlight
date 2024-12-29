#pragma once

namespace sunlight
{
    auto GetFileSize(std::ifstream& ifs) -> int64_t;
    auto ReadBinaryFile(const std::filesystem::path& filePath) -> std::vector<char>;
}
