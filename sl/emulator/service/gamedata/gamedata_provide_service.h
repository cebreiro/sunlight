#pragma once

namespace sunlight
{
    class SoxTableSet;
    class ItemDataProvider;
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

        auto GetItemDataProvider() const -> const ItemDataProvider&;

    private:
        const ServiceLocator& _serviceLocator;

        SharedPtrNotNull<SoxTableSet> _soxTableSet;
        SharedPtrNotNull<ItemDataProvider> _itemDataProvider;
    };
}
