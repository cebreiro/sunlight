#include "eventdeffuncserver.h"

#include "sl/data/sox/sox_file.h"

namespace sunlight::sox
{
    Eventdeffuncserver::Eventdeffuncserver(StreamReader<std::vector<char>::const_iterator>& reader)
    {
        index = reader.Read<int32_t>();
        classId = reader.Read<int32_t>();
        trigger = reader.Read<int32_t>();
        deffuncId = reader.Read<int32_t>();
    }

    void EventdeffuncserverTable::LoadFromFile(const std::filesystem::path& path)
    {
        SoxFile fileData = SoxFile::CreateFrom(path / "EVENTDEFFUNCSERVER.sox");
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

    auto EventdeffuncserverTable::Find(int32_t index) const -> const Eventdeffuncserver*
    {
        auto iter = _umap.find(index);

        return iter != _umap.end() ? iter->second : nullptr;
    }

    auto EventdeffuncserverTable::Get() const -> const std::vector<Eventdeffuncserver>&
    {
        return _vector;
    }
}
