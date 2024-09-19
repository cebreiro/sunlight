#include "gamedata_provide_service.h"

#include "sl/data/asset_data_provider.h"
#include "sl/emulator/game/data/sox_table_set.h"
#include "sl/emulator/service/gamedata/exp/exp_data_provider.h"
#include "sl/emulator/service/gamedata/item/item_data_provider.h"
#include "sl/emulator/service/gamedata/map/map_data_provider.h"
#include "sl/emulator/service/gamedata/skill/skill_data_provider.h"

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
        _assetDataProvider = std::make_shared<AssetDataProvider>(assetPath / "Soda.dat");
        _mapDataProvider = std::make_shared<MapDataProvider>(assetPath / "Zone");
        _soxTableSet = std::make_shared<SoxTableSet>(assetPath / "Misc");

        _itemDataProvider = std::make_shared<ItemDataProvider>(_serviceLocator, *_soxTableSet);
        _skillDataProvider = std::make_shared<SkillDataProvider>(_serviceLocator, *_soxTableSet);
        _expDataProvider = std::make_shared<ExpDataProvider>(*_soxTableSet);
    }

    auto GameDataProvideService::GetAssetDataProvider() const -> const AssetDataProvider&
    {
        assert(_assetDataProvider);

        return *_assetDataProvider;
    }

    auto GameDataProvideService::GetMapDataProvider() const -> const MapDataProvider&
    {
        assert(_mapDataProvider);

        return *_mapDataProvider;
    }

    auto GameDataProvideService::GetItemDataProvider() const -> const ItemDataProvider&
    {
        assert(_itemDataProvider);

        return *_itemDataProvider;
    }

    auto GameDataProvideService::GetSkillDataProvider() const -> const SkillDataProvider&
    {
        assert(_skillDataProvider);

        return *_skillDataProvider;
    }

    auto GameDataProvideService::GetExpDataProvider() const -> const ExpDataProvider&
    {
        assert(_expDataProvider);

        return *_expDataProvider;
    }
}
