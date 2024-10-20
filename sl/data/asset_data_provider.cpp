#include "asset_data_provider.h"

#include "sl/data/soda_dat.h"

namespace sunlight
{
    AssetDataProvider::AssetDataProvider(const std::filesystem::path& sodaPath)
        : _sodaDat(std::make_unique<SodaDat>(SodaDat::CreateFrom(sodaPath)))
    {
        for (const AssetData& data : _sodaDat->data | std::views::values)
        {
            _nameIndex.try_emplace(data.name, &data);
            _partIdIndex.emplace(data.partId, &data);
        }
    }

    AssetDataProvider::~AssetDataProvider()
    {
    }

    auto AssetDataProvider::Find(uint32_t id) const -> const AssetData*
    {
        const auto iter = _sodaDat->data.find(id);

        return iter != _sodaDat->data.end() ? &iter->second : nullptr;
    }

    auto AssetDataProvider::FindByName(const std::string& name) const -> const AssetData*
    {
        const auto iter = _nameIndex.find(name);

        return iter != _nameIndex.end() ? iter->second : nullptr;
    }
}
