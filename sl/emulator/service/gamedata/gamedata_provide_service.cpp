#include "gamedata_provide_service.h"

#include "sl/emulator/game/data/sox_table_set.h"
#include "sl/emulator/service/gamedata/item/item_data_provider.h"

namespace sunlight
{
    GameDataProvideService::GameDataProvideService(const ServiceLocator& serviceLocator)
        : _serviceLocator(serviceLocator)
    {
    }

    GameDataProvideService::~GameDataProvideService()
    {
    }

    auto GameDataProvideService::GetName() const -> std::string_view
    {
        return "game_data_provide_service";
    }

    void GameDataProvideService::Initialize(const std::filesystem::path& assetPath)
    {
        _soxTableSet = std::make_shared<SoxTableSet>(assetPath / "Misc");
        _itemDataProvider = std::make_shared<ItemDataProvider>(_serviceLocator, *_soxTableSet);
    }

    auto GameDataProvideService::GetItemDataProvider() const -> const ItemDataProvider&
    {
        assert(_itemDataProvider);

        return *_itemDataProvider;
    }
}
