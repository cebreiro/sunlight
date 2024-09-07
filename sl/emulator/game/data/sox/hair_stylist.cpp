#include "hair_stylist.h"

#include "sl/data/sox/sox_file.h"

namespace sunlight::sox
{
    HairStylist::HairStylist(StreamReader<std::vector<char>::const_iterator>& reader)
    {
        index = reader.Read<int32_t>();
        coloringPrice = reader.Read<int32_t>();
        id01 = reader.Read<int32_t>();
        sex01 = reader.Read<int32_t>();
        price01 = reader.Read<int32_t>();
        id02 = reader.Read<int32_t>();
        sex02 = reader.Read<int32_t>();
        price02 = reader.Read<int32_t>();
        id03 = reader.Read<int32_t>();
        sex03 = reader.Read<int32_t>();
        price03 = reader.Read<int32_t>();
        id04 = reader.Read<int32_t>();
        sex04 = reader.Read<int32_t>();
        price04 = reader.Read<int32_t>();
        id05 = reader.Read<int32_t>();
        sex05 = reader.Read<int32_t>();
        price05 = reader.Read<int32_t>();
        id06 = reader.Read<int32_t>();
        sex06 = reader.Read<int32_t>();
        price06 = reader.Read<int32_t>();
        id07 = reader.Read<int32_t>();
        sex07 = reader.Read<int32_t>();
        price07 = reader.Read<int32_t>();
        id08 = reader.Read<int32_t>();
        sex08 = reader.Read<int32_t>();
        price08 = reader.Read<int32_t>();
        id09 = reader.Read<int32_t>();
        sex09 = reader.Read<int32_t>();
        price09 = reader.Read<int32_t>();
        id10 = reader.Read<int32_t>();
        sex10 = reader.Read<int32_t>();
        price10 = reader.Read<int32_t>();
        id11 = reader.Read<int32_t>();
        sex11 = reader.Read<int32_t>();
        price11 = reader.Read<int32_t>();
        id12 = reader.Read<int32_t>();
        sex12 = reader.Read<int32_t>();
        price12 = reader.Read<int32_t>();
        id13 = reader.Read<int32_t>();
        sex13 = reader.Read<int32_t>();
        price13 = reader.Read<int32_t>();
        id14 = reader.Read<int32_t>();
        sex14 = reader.Read<int32_t>();
        price14 = reader.Read<int32_t>();
        id15 = reader.Read<int32_t>();
        sex15 = reader.Read<int32_t>();
        price15 = reader.Read<int32_t>();
        id16 = reader.Read<int32_t>();
        sex16 = reader.Read<int32_t>();
        price16 = reader.Read<int32_t>();
        id17 = reader.Read<int32_t>();
        sex17 = reader.Read<int32_t>();
        price17 = reader.Read<int32_t>();
        id18 = reader.Read<int32_t>();
        sex18 = reader.Read<int32_t>();
        price18 = reader.Read<int32_t>();
        id19 = reader.Read<int32_t>();
        sex19 = reader.Read<int32_t>();
        price19 = reader.Read<int32_t>();
        id20 = reader.Read<int32_t>();
        sex20 = reader.Read<int32_t>();
        price20 = reader.Read<int32_t>();
        id21 = reader.Read<int32_t>();
        sex21 = reader.Read<int32_t>();
        price21 = reader.Read<int32_t>();
        id22 = reader.Read<int32_t>();
        sex22 = reader.Read<int32_t>();
        price22 = reader.Read<int32_t>();
        id23 = reader.Read<int32_t>();
        sex23 = reader.Read<int32_t>();
        price23 = reader.Read<int32_t>();
        id24 = reader.Read<int32_t>();
        sex24 = reader.Read<int32_t>();
        price24 = reader.Read<int32_t>();
        id25 = reader.Read<int32_t>();
        sex25 = reader.Read<int32_t>();
        price25 = reader.Read<int32_t>();
        id26 = reader.Read<int32_t>();
        sex26 = reader.Read<int32_t>();
        price26 = reader.Read<int32_t>();
        id27 = reader.Read<int32_t>();
        sex27 = reader.Read<int32_t>();
        price27 = reader.Read<int32_t>();
        id28 = reader.Read<int32_t>();
        sex28 = reader.Read<int32_t>();
        price28 = reader.Read<int32_t>();
        id29 = reader.Read<int32_t>();
        sex29 = reader.Read<int32_t>();
        price29 = reader.Read<int32_t>();
        id30 = reader.Read<int32_t>();
        sex30 = reader.Read<int32_t>();
        price30 = reader.Read<int32_t>();
        id31 = reader.Read<int32_t>();
        sex31 = reader.Read<int32_t>();
        price31 = reader.Read<int32_t>();
        id32 = reader.Read<int32_t>();
        sex32 = reader.Read<int32_t>();
        price32 = reader.Read<int32_t>();
        id33 = reader.Read<int32_t>();
        sex33 = reader.Read<int32_t>();
        price33 = reader.Read<int32_t>();
        id34 = reader.Read<int32_t>();
        sex34 = reader.Read<int32_t>();
        price34 = reader.Read<int32_t>();
        id35 = reader.Read<int32_t>();
        sex35 = reader.Read<int32_t>();
        price35 = reader.Read<int32_t>();
        id36 = reader.Read<int32_t>();
        sex36 = reader.Read<int32_t>();
        price36 = reader.Read<int32_t>();
        id37 = reader.Read<int32_t>();
        sex37 = reader.Read<int32_t>();
        price37 = reader.Read<int32_t>();
        id38 = reader.Read<int32_t>();
        sex38 = reader.Read<int32_t>();
        price38 = reader.Read<int32_t>();
        id39 = reader.Read<int32_t>();
        sex39 = reader.Read<int32_t>();
        price39 = reader.Read<int32_t>();
        id40 = reader.Read<int32_t>();
        sex40 = reader.Read<int32_t>();
        price40 = reader.Read<int32_t>();
    }

    void HairStylistTable::LoadFromFile(const std::filesystem::path& path)
    {
        SoxFile fileData = SoxFile::CreateFrom(path / "HAIR_STYLIST.sox");
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

    auto HairStylistTable::Find(int32_t index) const -> const HairStylist*
    {
        auto iter = _umap.find(index);

        return iter != _umap.end() ? iter->second : nullptr;
    }

    auto HairStylistTable::Get() const -> const std::vector<HairStylist>&
    {
        return _vector;
    }
}
