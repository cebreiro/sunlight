#pragma once

namespace sunlight
{
    struct SodaDat;
    struct AssetData;
}

namespace sunlight
{
    class AssetDataProvider
    {
    public:
        AssetDataProvider(const AssetDataProvider& other) = delete;
        AssetDataProvider(AssetDataProvider&& other) noexcept = delete;
        AssetDataProvider& operator=(const AssetDataProvider& other) = delete;
        AssetDataProvider& operator=(AssetDataProvider&& other) noexcept = delete;

    public:
        explicit AssetDataProvider(const std::filesystem::path& assetPath);
        ~AssetDataProvider();

        auto Find(uint32_t id) const -> const AssetData*;
        auto FindByName(const std::string& name) const -> const AssetData*;
        inline auto FindByPartId(uint32_t id) const;

    private:
        UniquePtrNotNull<SodaDat> _sodaDat;
        std::unordered_map<std::string, PtrNotNull<const AssetData>> _nameIndex;
        std::unordered_multimap<uint32_t, PtrNotNull<const AssetData>> _partIdIndex;
    };

    inline auto AssetDataProvider::FindByPartId(uint32_t id) const
    {
        const auto [begin, end] = _partIdIndex.equal_range(id);

        return std::ranges::subrange(begin, end);
    }
}
