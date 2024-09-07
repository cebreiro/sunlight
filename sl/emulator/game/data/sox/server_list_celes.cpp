#include "server_list_celes.h"

#include "sl/data/sox/sox_file.h"

namespace sunlight::sox
{
    ServerListCeles::ServerListCeles(StreamReader<std::vector<char>::const_iterator>& reader)
    {
        index = reader.Read<int32_t>();
        name = reader.ReadString(reader.Read<uint16_t>());
        desc = reader.ReadString(reader.Read<uint16_t>());
        ip = reader.ReadString(reader.Read<uint16_t>());
    }

    void ServerListCelesTable::LoadFromFile(const std::filesystem::path& path)
    {
        SoxFile fileData = SoxFile::CreateFrom(path / "SERVER_LIST_CELES.sox");
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

    auto ServerListCelesTable::Find(int32_t index) const -> const ServerListCeles*
    {
        auto iter = _umap.find(index);

        return iter != _umap.end() ? iter->second : nullptr;
    }

    auto ServerListCelesTable::Get() const -> const std::vector<ServerListCeles>&
    {
        return _vector;
    }
}
