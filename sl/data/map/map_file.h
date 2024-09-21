#pragma once

namespace sunlight
{
    struct MapStage;
}

namespace sunlight
{
    struct MapFile
    {
        MapFile(const MapFile& other) = delete;
        MapFile& operator=(const MapFile& other) = delete;

    public:
        MapFile() = default;
        MapFile(MapFile&& other) noexcept = default;
        MapFile& operator=(MapFile&& other) noexcept = default;

        ~MapFile();

        std::array<char, 48> comment = {};
        int32_t unk1 = 0;

        std::vector<UniquePtrNotNull<MapStage>> stages;

        static auto CreateFrom(const std::filesystem::path& filePath) -> MapFile;
    };
}
