#include "server_list_test.h"

#include "sl/data/sox/sox_file.h"

namespace sunlight::sox
{
    ServerListTest::ServerListTest(StreamReader<std::vector<char>::const_iterator>& reader)
    {
        index = reader.Read<int32_t>();
        name = reader.ReadString(reader.Read<uint16_t>());
        desc = reader.ReadString(reader.Read<uint16_t>());
        ip = reader.ReadString(reader.Read<uint16_t>());
    }

    void ServerListTestTable::LoadFromFile(const std::filesystem::path& path)
    {
        SoxFile fileData = SoxFile::CreateFrom(path / "SERVER_LIST_TEST.sox");
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

    auto ServerListTestTable::Find(int32_t index) const -> const ServerListTest*
    {
        auto iter = _umap.find(index);

        return iter != _umap.end() ? iter->second : nullptr;
    }

    auto ServerListTestTable::Get() const -> const std::vector<ServerListTest>&
    {
        return _vector;
    }
}
