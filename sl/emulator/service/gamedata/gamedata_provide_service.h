#pragma once
#include "sl/data/sox/sox_table_interface.h"
#include "sl/emulator/game/data/sox_table_set.h"

namespace sunlight
{
    class AssetDataProvider;
    class MapDataProvider;
    class ItemDataProvider;
    class SkillDataProvider;
    class ExpDataProvider;
}

namespace sunlight
{
    class GameDataProvideService final
        : public IService
        , public std::enable_shared_from_this<GameDataProvideService>
    {
    public:
        explicit GameDataProvideService(const ServiceLocator& serviceLocator);
        ~GameDataProvideService();

        auto GetName() const -> std::string_view override;

    public:
        void Initialize(const std::filesystem::path& assetPath);

        template <typename T> requires std::derived_from<T, sox::ISoxTable>
        auto Get() const -> const T&;

        auto GetAssetDataProvider() const -> const AssetDataProvider&;
        auto GetMapDataProvider() const -> const MapDataProvider&;
        auto GetItemDataProvider() const -> const ItemDataProvider&;
        auto GetSkillDataProvider() const -> const SkillDataProvider&;
        auto GetExpDataProvider() const -> const ExpDataProvider&;

    private:
        const ServiceLocator& _serviceLocator;

        SharedPtrNotNull<AssetDataProvider> _assetDataProvider;
        SharedPtrNotNull<MapDataProvider> _mapDataProvider;
        SharedPtrNotNull<SoxTableSet> _soxTableSet;
        SharedPtrNotNull<ItemDataProvider> _itemDataProvider;
        SharedPtrNotNull<SkillDataProvider> _skillDataProvider;
        SharedPtrNotNull<ExpDataProvider> _expDataProvider;
    };

    template <typename T> requires std::derived_from<T, sox::ISoxTable>
    auto GameDataProvideService::Get() const -> const T&
    {
        return _soxTableSet->Get<T>();
    }
}
