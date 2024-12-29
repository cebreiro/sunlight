#include "server_command_item.h"

#include "sl/generator/game/component/item_position_component.h"
#include "sl/generator/game/component/scene_object_component.h"
#include "sl/generator/game/entity/game_item.h"
#include "sl/generator/game/entity/game_stored_item.h"
#include "sl/generator/game/message/creator/item_archive_message_creator.h"
#include "sl/generator/game/system/item_archive_system.h"
#include "sl/generator/game/system/scene_object_system.h"
#include "sl/generator/game/system/server_command_system.h"
#include "sl/generator/game/zone/service/game_entity_id_publisher.h"
#include "sl/generator/service/gamedata/gamedata_provide_service.h"
#include "sl/generator/service/gamedata/item/item_data_provider.h"

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
        const bool added = _system.Get<ItemArchiveSystem>().AddItem(player, itemId, quantity);

        return added;
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

        sceneObjectSystem.SpawnItem(std::move(item), playerPos, playerPos, std::nullopt);

        return true;
    }
}
