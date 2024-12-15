#include "external_jamming_list.h"

#include "sl/data/sox/sox_file.h"

namespace sunlight::sox
{
    ExternalJammingList::ExternalJammingList(StreamReader<std::vector<char>::const_iterator>& reader)
    {
        index = reader.Read<int32_t>();
        jammingType = reader.Read<int32_t>();
        className = reader.ReadString(reader.Read<uint16_t>());
        windowName = reader.ReadString(reader.Read<uint16_t>());
        fileChecksum = reader.Read<int32_t>();
        fileSize = reader.Read<int32_t>();
        numSpare1 = reader.Read<int32_t>();
        internalName = reader.ReadString(reader.Read<uint16_t>());
        legalCopyright = reader.ReadString(reader.Read<uint16_t>());
        originalFilename = reader.ReadString(reader.Read<uint16_t>());
        productionVersion = reader.ReadString(reader.Read<uint16_t>());
        comments = reader.ReadString(reader.Read<uint16_t>());
        privateBuild = reader.ReadString(reader.Read<uint16_t>());
        specialBuild = reader.ReadString(reader.Read<uint16_t>());
        spareField = reader.ReadString(reader.Read<uint16_t>());
    }

    void ExternalJammingListTable::LoadFromFile(const std::filesystem::path& path)
    {
        SoxFile fileData = SoxFile::CreateFrom(path / "EXTERNAL_JAMMING_LIST.sox");
        StreamReader<std::vector<char>::const_iterator> reader(fileData.gameData.begin(), fileData.gameData.end());

        _vector.reserve(fileData.rowCount);
        for (int64_t i = 0; i < fileData.rowCount; ++i)
        {
            _vector.emplace_back(reader);
        }

        for (auto& data : _vector)
        {
            _umap[data.index] = &data;
        }
    }

    auto ExternalJammingListTable::Find(int32_t index) const -> const ExternalJammingList*
    {
        auto iter = _umap.find(index);

        return iter != _umap.end() ? iter->second : nullptr;
    }

    auto ExternalJammingListTable::Get() const -> const std::vector<ExternalJammingList>&
    {
        return _vector;
    }
}
