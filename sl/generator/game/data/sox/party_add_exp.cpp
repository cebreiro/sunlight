#include "party_add_exp.h"

#include "sl/data/sox/sox_file.h"

namespace sunlight::sox
{
    PartyAddExp::PartyAddExp(StreamReader<std::vector<char>::const_iterator>& reader)
    {
        index = reader.Read<int32_t>();
        addExpFactor = reader.Read<float>();
    }

    void PartyAddExpTable::LoadFromFile(const std::filesystem::path& path)
    {
        SoxFile fileData = SoxFile::CreateFrom(path / "party_add_exp.sox");
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

    auto PartyAddExpTable::Find(int32_t index) const -> const PartyAddExp*
    {
        auto iter = _umap.find(index);

        return iter != _umap.end() ? iter->second : nullptr;
    }

    auto PartyAddExpTable::Get() const -> const std::vector<PartyAddExp>&
    {
        return _vector;
    }
}
