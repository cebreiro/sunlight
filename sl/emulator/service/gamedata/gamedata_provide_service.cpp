#include "gamedata_provide_service.h"

#include "sl/data/asset_data_provider.h"
#include "sl/emulator/game/data/sox_table_set.h"
#include "sl/emulator/service/gamedata/exp/exp_data_provider.h"
#include "sl/emulator/service/gamedata/item/item_data_provider.h"
#include "sl/emulator/service/gamedata/item_mix/item_mix_data_provider.h"
#include "sl/emulator/service/gamedata/map/map_data_provider.h"
#include "sl/emulator/service/gamedata/shop/npc_shop_data_provider.h"
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
        _assertPath = assetPath;
        _scriptPath = assetPath / "Script";

        _assetDataProvider = std::make_shared<AssetDataProvider>(assetPath / "Soda.dat");
        _mapDataProvider = std::make_shared<MapDataProvider>(assetPath / "Zone");
        _soxTableSet = std::make_shared<SoxTableSet>(assetPath / "Misc");

        _itemDataProvider = std::make_shared<ItemDataProvider>(_serviceLocator, *_soxTableSet);
        _itemMixDataProvider = std::make_shared<ItemMixDataProvider>(_serviceLocator, *_soxTableSet, *_itemDataProvider);
        _skillDataProvider = std::make_shared<SkillDataProvider>(_serviceLocator, *_soxTableSet);
        _expDataProvider = std::make_shared<ExpDataProvider>(*_soxTableSet);
        _npcShopDataProvider = std::make_shared<NPCShopDataProvider>(*_soxTableSet);
    }

    auto GameDataProvideService::GetAssetPath() const -> const std::filesystem::path&
    {
        return _assertPath;
    }

    auto GameDataProvideService::GetScriptPath() const -> const std::filesystem::path&
    {
        return _scriptPath;
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

    auto GameDataProvideService::GetItemMixDataProvider() const -> const ItemMixDataProvider&
    {
        assert(_itemMixDataProvider);

        return *_itemMixDataProvider;
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

    auto GameDataProvideService::GetNPCShopDataProvider() const -> const NPCShopDataProvider&
    {
        assert(_npcShopDataProvider);

        return *_npcShopDataProvider;
    }
}
