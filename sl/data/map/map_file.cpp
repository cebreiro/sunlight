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

        for (int32_t i = 0; i < count; ++i)
        {
            const int64_t current = reader.GetReadSize();
            const MapStage& stageData = *mapFile.stages.emplace_back(std::make_unique<MapStage>(reader));

            // skip unk bytes
            reader.SetOffset(current + stageData.size + 36);
        }

        return mapFile;
    }
}
