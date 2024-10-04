#include "server_command_item.h"

#include "sl/emulator/game/component/scene_object_component.h"
#include "sl/emulator/game/entity/game_item.h"
#include "sl/emulator/game/entity/game_stored_item.h"
#include "sl/emulator/game/system/item_archive_system.h"
#include "sl/emulator/game/system/scene_object_system.h"
#include "sl/emulator/game/system/server_command_system.h"
#include "sl/emulator/game/zone/service/game_entity_id_publisher.h"
#include "sl/emulator/service/gamedata/gamedata_provide_service.h"
#include "sl/emulator/service/gamedata/item/item_data_provider.h"

namespace sunlight
{
    ServerCommandItemAdd::ServerCommandItemAdd(ServerCommandSystem& system)
        : _system(system)
    {
    }

    auto ServerCommandItemAdd::GetName() const -> std::string_view
    {
        return "item_add";
    }

    auto ServerCommandItemAdd::GetRequiredGmLevel() const -> int8_t
    {
        return 0;
    }

    bool ServerCommandItemAdd::Execute(GamePlayer& player, int32_t itemId, int32_t quantity) const
    {
        return _system.Get<ItemArchiveSystem>().AddItem(player, itemId, quantity);
    }

    ServerCommandItemGain::ServerCommandItemGain(ServerCommandSystem& system)
        : _system(system)
    {
    }

    auto ServerCommandItemGain::GetName() const -> std::string_view
    {
        return "item_gain";
    }

    auto ServerCommandItemGain::GetRequiredGmLevel() const -> int8_t
    {
        return 0;
    }

    bool ServerCommandItemGain::Execute(GamePlayer& player, int32_t itemId, int32_t quantity) const
    {
        int32_t addQuantity = 0;

        return _system.Get<ItemArchiveSystem>().GainItem(player, itemId, quantity, addQuantity);
    }

    ServerCommandItemSpawn::ServerCommandItemSpawn(ServerCommandSystem& system)
        : _system(system)
    {
    }

    auto ServerCommandItemSpawn::GetName() const -> std::string_view
    {
        return "item_spawn";
    }

    auto ServerCommandItemSpawn::GetRequiredGmLevel() const -> int8_t
    {
        return 0;
    }

    bool ServerCommandItemSpawn::Execute(GamePlayer& player, int32_t itemId, int32_t quantity) const
    {
        const ItemDataProvider& itemDataProvider = _system.GetServiceLocator().Get<GameDataProvideService>().GetItemDataProvider();
        const ItemData* itemData = itemDataProvider.Find(itemId);

        if (!itemData)
        {
            return false;
        }

        auto item = std::make_shared<GameItem>(_system.GetServiceLocator().Get<GameEntityIdPublisher>(),
            *itemData, std::clamp(quantity, 1, itemData->GetMaxOverlapCount()));
 
        const Eigen::Vector2f& playerPos = player.GetSceneObjectComponent().GetPosition();

        SceneObjectSystem& sceneObjectSystem = _system.Get<SceneObjectSystem>();

        sceneObjectSystem.SpawnItem(std::move(item), playerPos, playerPos);

        return true;
    }

    ServerCommandItemStoredSpawn::ServerCommandItemStoredSpawn(ServerCommandSystem& system)
        : _system(system)
    {
    }

    auto ServerCommandItemStoredSpawn::GetName() const -> std::string_view
    {
        return "item_stored_spawn";
    }

    auto ServerCommandItemStoredSpawn::GetRequiredGmLevel() const -> int8_t
    {
        return 0;
    }

    bool ServerCommandItemStoredSpawn::Execute(GamePlayer& player, int32_t itemId, int32_t quantity, int32_t groupId) const
    {
        const ItemDataProvider& itemDataProvider = _system.GetServiceLocator().Get<GameDataProvideService>().GetItemDataProvider();
        const ItemData* itemData = itemDataProvider.Find(itemId);

        if (!itemData)
        {
            return false;
        }

        auto item = std::make_shared<GameStoredItem>(_system.GetServiceLocator().Get<GameEntityIdPublisher>(),
            *itemData, std::clamp(quantity, 1, itemData->GetMaxOverlapCount()), groupId, 1000);

        Eigen::Vector2f position = player.GetSceneObjectComponent().GetPosition();
        position.x() += 30.f;
        position.y() += 30.f;

        //SceneObjectSystem& sceneObjectSystem = _system.Get<SceneObjectSystem>();

        //sceneObjectSystem.SpawnItem(std::move(item));

        return true;
    }
}
