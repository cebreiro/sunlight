#include "ability_part2.h"

#include "sl/data/sox/sox_file.h"

namespace sunlight::sox
{
    AbilityPart2::AbilityPart2(StreamReader<std::vector<char>::const_iterator>& reader)
    {
        index = reader.Read<int32_t>();
        calc1Type = reader.Read<int32_t>();
        calc1Unit = reader.Read<int32_t>();
        factor101 = reader.Read<float>();
        factor102 = reader.Read<float>();
        factor103 = reader.Read<float>();
        factor104 = reader.Read<float>();
        factor105 = reader.Read<float>();
        factor106 = reader.Read<float>();
        factor107 = reader.Read<float>();
        factor108 = reader.Read<float>();
        factor109 = reader.Read<float>();
        factor110 = reader.Read<float>();
        factor111 = reader.Read<float>();
        factor112 = reader.Read<float>();
        factor113 = reader.Read<float>();
        factor114 = reader.Read<float>();
        factor115 = reader.Read<float>();
        b11 = reader.Read<float>();
        b12 = reader.Read<float>();
        b13 = reader.Read<float>();
        b14 = reader.Read<float>();
        b15 = reader.Read<float>();
        b16 = reader.Read<float>();
        b17 = reader.Read<float>();
        b18 = reader.Read<float>();
        b19 = reader.Read<float>();
        b110 = reader.Read<float>();
        b111 = reader.Read<float>();
        b112 = reader.Read<float>();
        b113 = reader.Read<float>();
        b114 = reader.Read<float>();
        b115 = reader.Read<float>();
        calc2Type = reader.Read<int32_t>();
        calc2Unit = reader.Read<int32_t>();
        factor201 = reader.Read<float>();
        factor202 = reader.Read<float>();
        factor203 = reader.Read<float>();
        factor204 = reader.Read<float>();
        factor205 = reader.Read<float>();
        factor206 = reader.Read<float>();
        factor207 = reader.Read<float>();
        factor208 = reader.Read<float>();
        factor209 = reader.Read<float>();
        factor210 = reader.Read<float>();
        factor211 = reader.Read<float>();
        factor212 = reader.Read<float>();
        factor213 = reader.Read<float>();
        factor214 = reader.Read<float>();
        factor215 = reader.Read<float>();
        b21 = reader.Read<float>();
        b22 = reader.Read<float>();
        b23 = reader.Read<float>();
        b24 = reader.Read<float>();
        b25 = reader.Read<float>();
        b26 = reader.Read<float>();
        b27 = reader.Read<float>();
        b28 = reader.Read<float>();
        b29 = reader.Read<float>();
        b210 = reader.Read<float>();
        b211 = reader.Read<float>();
        b212 = reader.Read<float>();
        b213 = reader.Read<float>();
        b214 = reader.Read<float>();
        b215 = reader.Read<float>();
        calc3Type = reader.Read<int32_t>();
        calc3Unit = reader.Read<int32_t>();
        factor301 = reader.Read<float>();
        factor302 = reader.Read<float>();
        factor303 = reader.Read<float>();
        factor304 = reader.Read<float>();
        factor305 = reader.Read<float>();
        factor306 = reader.Read<float>();
        factor307 = reader.Read<float>();
        factor308 = reader.Read<float>();
        factor309 = reader.Read<float>();
        factor310 = reader.Read<float>();
        factor311 = reader.Read<float>();
        factor312 = reader.Read<float>();
        factor313 = reader.Read<float>();
        factor314 = reader.Read<float>();
        factor315 = reader.Read<float>();
        b31 = reader.Read<float>();
        b32 = reader.Read<float>();
        b33 = reader.Read<float>();
        b34 = reader.Read<float>();
        b35 = reader.Read<float>();
        b36 = reader.Read<float>();
        b37 = reader.Read<float>();
        b38 = reader.Read<float>();
        b39 = reader.Read<float>();
        b310 = reader.Read<float>();
        b311 = reader.Read<float>();
        b312 = reader.Read<float>();
        b313 = reader.Read<float>();
        b314 = reader.Read<float>();
        b315 = reader.Read<float>();
    }

    void AbilityPart2Table::LoadFromFile(const std::filesystem::path& path)
    {
        SoxFile fileData = SoxFile::CreateFrom(path / "ABILITY_PART2.sox");
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

    auto AbilityPart2Table::Find(int32_t index) const -> const AbilityPart2*
    {
        auto iter = _umap.find(index);

        return iter != _umap.end() ? iter->second : nullptr;
    }

    auto AbilityPart2Table::Get() const -> const std::vector<AbilityPart2>&
    {
        return _vector;
    }
}
