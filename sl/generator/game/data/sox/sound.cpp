#include "sound.h"

#include "sl/data/sox/sox_file.h"

namespace sunlight::sox
{
    Sound::Sound(StreamReader<std::vector<char>::const_iterator>& reader)
    {
        index = reader.Read<int32_t>();
        soundKey1Time = reader.Read<int32_t>();
        soundKey1Wav = reader.Read<int32_t>();
        soundKey2Time = reader.Read<int32_t>();
        soundKey2Wav = reader.Read<int32_t>();
        soundKey3Time = reader.Read<int32_t>();
        soundKey3Wav = reader.Read<int32_t>();
        soundKey4Time = reader.Read<int32_t>();
        soundKey4Wav = reader.Read<int32_t>();
        soundKey5Time = reader.Read<int32_t>();
        soundKey5Wav = reader.Read<int32_t>();
        soundKey6Time = reader.Read<int32_t>();
        soundKey6Wav = reader.Read<int32_t>();
        soundKey7Time = reader.Read<int32_t>();
        soundKey7Wav = reader.Read<int32_t>();
        soundKey8Time = reader.Read<int32_t>();
        soundKey8Wav = reader.Read<int32_t>();
    }

    void SoundTable::LoadFromFile(const std::filesystem::path& path)
    {
        SoxFile fileData = SoxFile::CreateFrom(path / "sound.sox");
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

    auto SoundTable::Find(int32_t index) const -> const Sound*
    {
        auto iter = _umap.find(index);

        return iter != _umap.end() ? iter->second : nullptr;
    }

    auto SoundTable::Get() const -> const std::vector<Sound>&
    {
        return _vector;
    }
}
