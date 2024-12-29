#include "condition_action.h"

#include "sl/data/sox/sox_file.h"

namespace sunlight::sox
{
    ConditionAction::ConditionAction(StreamReader<std::vector<char>::const_iterator>& reader)
    {
        index = reader.Read<int32_t>();
        groupId = reader.Read<int32_t>();
        partId = reader.Read<int32_t>();
        position = reader.Read<int32_t>();
    }

    void ConditionActionTable::LoadFromFile(const std::filesystem::path& path)
    {
        SoxFile fileData = SoxFile::CreateFrom(path / "CONDITION_ACTION.sox");
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

    auto ConditionActionTable::Find(int32_t index) const -> const ConditionAction*
    {
        auto iter = _umap.find(index);

        return iter != _umap.end() ? iter->second : nullptr;
    }

    auto ConditionActionTable::Get() const -> const std::vector<ConditionAction>&
    {
        return _vector;
    }
}
