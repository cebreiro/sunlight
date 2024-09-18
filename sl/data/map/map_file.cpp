#include "map_file.h"

#include "sl/data/file_util.h"
#include "sl/data/map/map_stage.h"

namespace sunlight
{
    MapFile::~MapFile()
    {
    }

    auto MapFile::CreateFrom(const std::filesystem::path& filePath) -> MapFile
    {
        std::vector<char> fileData = ReadBinaryFile(filePath);
        StreamReader<std::vector<char>::const_iterator> reader(fileData.begin(), fileData.end());

        MapFile mapFile;

        reader.ReadBuffer(mapFile.comment.data(), std::ssize(mapFile.comment));
        mapFile.unk1 = reader.Read<int32_t>();

        const int32_t count = reader.Read<int32_t>();
        mapFile.stages.reserve(count);
        mapFile.unk2.reserve(count);

        for (int32_t i = 0; i < count; ++i)
        {
            mapFile.stages.emplace_back(std::make_unique<MapStage>(reader));

            std::array<char, 36>& unk = mapFile.unk2.emplace_back();
            reader.ReadBuffer(unk.data(), std::ssize(unk));
        }

        return mapFile;
    }
}
