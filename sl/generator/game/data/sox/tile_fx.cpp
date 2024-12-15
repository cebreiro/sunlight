#include "tile_fx.h"

#include "sl/data/sox/sox_file.h"

namespace sunlight::sox
{
    TileFx::TileFx(StreamReader<std::vector<char>::const_iterator>& reader)
    {
        index = reader.Read<int32_t>();
        enterolBellsyurot = reader.Read<int32_t>();
        enterolEnterance = reader.Read<int32_t>();
        enterolTolluen = reader.Read<int32_t>();
        enterolTrendol = reader.Read<int32_t>();
        enterolDesert = reader.Read<int32_t>();
        enterolMountain = reader.Read<int32_t>();
        enterolGrass = reader.Read<int32_t>();
        enterolWilderness = reader.Read<int32_t>();
        enterolMineField = reader.Read<int32_t>();
        enterolBeastDen = reader.Read<int32_t>();
        enterolAntCavern = reader.Read<int32_t>();
        enterolEnterolMaze = reader.Read<int32_t>();
        enterolUnderground = reader.Read<int32_t>();
        enterolLaboratory = reader.Read<int32_t>();
        enterolMestaruin = reader.Read<int32_t>();
        enterolTolluenruin = reader.Read<int32_t>();
    }

    void TileFxTable::LoadFromFile(const std::filesystem::path& path)
    {
        SoxFile fileData = SoxFile::CreateFrom(path / "TILE_FX.sox");
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

    auto TileFxTable::Find(int32_t index) const -> const TileFx*
    {
        auto iter = _umap.find(index);

        return iter != _umap.end() ? iter->second : nullptr;
    }

    auto TileFxTable::Get() const -> const std::vector<TileFx>&
    {
        return _vector;
    }
}
