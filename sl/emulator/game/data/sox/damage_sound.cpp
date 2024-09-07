#include "damage_sound.h"

#include "sl/data/sox/sox_file.h"

namespace sunlight::sox
{
    DamageSound::DamageSound(StreamReader<std::vector<char>::const_iterator>& reader)
    {
        index = reader.Read<int32_t>();
        matHard = reader.Read<int32_t>();
        matNormal = reader.Read<int32_t>();
        matSoft = reader.Read<int32_t>();
        matReserved1 = reader.Read<int32_t>();
        matReserved2 = reader.Read<int32_t>();
        matReserved3 = reader.Read<int32_t>();
    }

    void DamageSoundTable::LoadFromFile(const std::filesystem::path& path)
    {
        SoxFile fileData = SoxFile::CreateFrom(path / "DAMAGE_SOUND.sox");
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

    auto DamageSoundTable::Find(int32_t index) const -> const DamageSound*
    {
        auto iter = _umap.find(index);

        return iter != _umap.end() ? iter->second : nullptr;
    }

    auto DamageSoundTable::Get() const -> const std::vector<DamageSound>&
    {
        return _vector;
    }
}
