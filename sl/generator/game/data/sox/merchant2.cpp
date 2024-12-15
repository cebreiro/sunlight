#include "merchant2.h"

#include "sl/data/sox/sox_file.h"

namespace sunlight::sox
{
    Merchant2::Merchant2(StreamReader<std::vector<char>::const_iterator>& reader)
    {
        index = reader.Read<int32_t>();
        name = reader.ReadString(reader.Read<uint16_t>());
        type = reader.Read<int32_t>();
        buyingFactor = reader.Read<int32_t>();
        sellingFactor = reader.Read<int32_t>();
        item01 = reader.Read<int32_t>();
        item02 = reader.Read<int32_t>();
        item03 = reader.Read<int32_t>();
        item04 = reader.Read<int32_t>();
        item05 = reader.Read<int32_t>();
        item06 = reader.Read<int32_t>();
        item07 = reader.Read<int32_t>();
        item08 = reader.Read<int32_t>();
        item09 = reader.Read<int32_t>();
        item10 = reader.Read<int32_t>();
        item11 = reader.Read<int32_t>();
        item12 = reader.Read<int32_t>();
        item13 = reader.Read<int32_t>();
        item14 = reader.Read<int32_t>();
        item15 = reader.Read<int32_t>();
        item16 = reader.Read<int32_t>();
        item17 = reader.Read<int32_t>();
        item18 = reader.Read<int32_t>();
        item19 = reader.Read<int32_t>();
        item20 = reader.Read<int32_t>();
        item21 = reader.Read<int32_t>();
        item22 = reader.Read<int32_t>();
        item23 = reader.Read<int32_t>();
        item24 = reader.Read<int32_t>();
        item25 = reader.Read<int32_t>();
        item26 = reader.Read<int32_t>();
        item27 = reader.Read<int32_t>();
        item28 = reader.Read<int32_t>();
        item29 = reader.Read<int32_t>();
        item30 = reader.Read<int32_t>();
        item31 = reader.Read<int32_t>();
        item32 = reader.Read<int32_t>();
        item33 = reader.Read<int32_t>();
        item34 = reader.Read<int32_t>();
        item35 = reader.Read<int32_t>();
        item36 = reader.Read<int32_t>();
        item37 = reader.Read<int32_t>();
        item38 = reader.Read<int32_t>();
        item39 = reader.Read<int32_t>();
        item40 = reader.Read<int32_t>();
        item41 = reader.Read<int32_t>();
        item42 = reader.Read<int32_t>();
        item43 = reader.Read<int32_t>();
        item44 = reader.Read<int32_t>();
        item45 = reader.Read<int32_t>();
        item46 = reader.Read<int32_t>();
        item47 = reader.Read<int32_t>();
        item48 = reader.Read<int32_t>();
        item49 = reader.Read<int32_t>();
        item50 = reader.Read<int32_t>();
        item51 = reader.Read<int32_t>();
        item52 = reader.Read<int32_t>();
        item53 = reader.Read<int32_t>();
        item54 = reader.Read<int32_t>();
        item55 = reader.Read<int32_t>();
        item56 = reader.Read<int32_t>();
        item57 = reader.Read<int32_t>();
        item58 = reader.Read<int32_t>();
        item59 = reader.Read<int32_t>();
        item60 = reader.Read<int32_t>();
        item61 = reader.Read<int32_t>();
        item62 = reader.Read<int32_t>();
        item63 = reader.Read<int32_t>();
        item64 = reader.Read<int32_t>();
        item65 = reader.Read<int32_t>();
        item66 = reader.Read<int32_t>();
        item67 = reader.Read<int32_t>();
        item68 = reader.Read<int32_t>();
        item69 = reader.Read<int32_t>();
        item70 = reader.Read<int32_t>();
        item71 = reader.Read<int32_t>();
        item72 = reader.Read<int32_t>();
        item73 = reader.Read<int32_t>();
        item74 = reader.Read<int32_t>();
        item75 = reader.Read<int32_t>();
        item76 = reader.Read<int32_t>();
        item77 = reader.Read<int32_t>();
        item78 = reader.Read<int32_t>();
        item79 = reader.Read<int32_t>();
        item80 = reader.Read<int32_t>();
    }

    void Merchant2Table::LoadFromFile(const std::filesystem::path& path)
    {
        SoxFile fileData = SoxFile::CreateFrom(path / "merchant2.sox");
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

    auto Merchant2Table::Find(int32_t index) const -> const Merchant2*
    {
        auto iter = _umap.find(index);

        return iter != _umap.end() ? iter->second : nullptr;
    }

    auto Merchant2Table::Get() const -> const std::vector<Merchant2>&
    {
        return _vector;
    }
}
