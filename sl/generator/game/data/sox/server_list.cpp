#include "server_list.h"

#include "sl/data/sox/sox_file.h"

namespace sunlight::sox
{
    ServerList::ServerList(StreamReader<std::vector<char>::const_iterator>& reader)
    {
        index = reader.Read<int32_t>();
        name = reader.ReadString(reader.Read<uint16_t>());
        desc = reader.ReadString(reader.Read<uint16_t>());
        ip = reader.ReadString(reader.Read<uint16_t>());
    }

    void ServerListTable::LoadFromFile(const std::filesystem::path& path)
    {
        SoxFile fileData = SoxFile::CreateFrom(path / "SERVER_LIST.sox");
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

    auto ServerListTable::Find(int32_t index) const -> const ServerList*
    {
        auto iter = _umap.find(index);

        return iter != _umap.end() ? iter->second : nullptr;
    }

    auto ServerListTable::Get() const -> const std::vector<ServerList>&
    {
        return _vector;
    }
}
