#include "nes_file.h"

#include "sl/data/file_util.h"

namespace sunlight
{
    auto NesFile::CreateFrom(const std::filesystem::path& filePath) -> NesFile
    {
        std::vector<char> fileData = ReadBinaryFile(filePath);
        StreamReader<std::vector<char>::const_iterator> reader(fileData.begin(), fileData.end());

        NesFile nesFile;
        nesFile.unk1 = reader.Read<int32_t>();
        nesFile.unk2 = reader.Read<int32_t>();

        const int32_t scriptCount = reader.Read<int32_t>();
        nesFile.scripts.reserve(scriptCount);
        nesFile.scriptIdIndex.reserve(scriptCount);

        for (int32_t i = 0; i < scriptCount; ++i)
        {
            const NesScript& script = nesFile.scripts.emplace_back(reader);

            if (!nesFile.scriptIdIndex.try_emplace(script.id, &script).second)
            {
                throw std::runtime_error(fmt::format("fail to insert nes script to index. duplicated id. id: {}", script.id));
            }
        }

        nesFile.unk3 = reader.Read<int32_t>();

        const int32_t scriptCallCount = reader.Read<int32_t>();
        nesFile.scriptCalls.reserve(scriptCallCount);
        nesFile.callerIdIndex.reserve(scriptCallCount);

        for (int32_t i = 0; i < scriptCallCount; ++i)
        {
            const NesScriptCall& scriptCall = nesFile.scriptCalls.emplace_back(reader);

            if (!nesFile.callerIdIndex.try_emplace(scriptCall.callerId, &scriptCall).second)
            {
                throw std::runtime_error(fmt::format("fail to insert nes script caller to index. duplicated caller id. id: {}", scriptCall.callerId));
            }
        }

        return nesFile;
    }
}
