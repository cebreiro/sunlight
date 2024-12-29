#include "server_command_npc.h"

#include "sl/generator/game/component/npc_item_shop_component.h"
#include "sl/generator/game/component/player_npc_shop_component.h"
#include "sl/generator/game/entity/game_npc.h"
#include "sl/generator/game/entity/game_player.h"
#include "sl/generator/game/system/npc_shop_system.h"
#include "sl/generator/game/system/server_command_system.h"

namespace sunlight
{
    ServerCommandNPCShopRoll::ServerCommandNPCShopRoll(ServerCommandSystem& system)
        : _system(system)
    {
    }

    auto ServerCommandNPCShopRoll::GetName() const -> std::string_view
    {
        return "npc_shop_roll";
    }

    auto ServerCommandNPCShopRoll::GetRequiredGmLevel() const -> int8_t
    {
        return 0;
    }

    bool ServerCommandNPCShopRoll::Execute(GamePlayer& player) const
    {
        const PlayerNPCShopComponent& playerNPCShopComponent = player.GetNPCShopComponent();

        if (!playerNPCShopComponent.HasShoppingNPC())
        {
            return false;
        }


        return _system.Get<NPCShopSystem>().Roll(playerNPCShopComponent.GetShoppingNPC());
    }

    ServerCommandNPCShopDebugSlot::ServerCommandNPCShopDebugSlot(ServerCommandSystem& system)
        : _system(system)
    {
    }

    auto ServerCommandNPCShopDebugSlot::GetName() const -> std::string_view
    {
        return "npc_shop_debug_slot";
    }

    auto ServerCommandNPCShopDebugSlot::GetRequiredGmLevel() const -> int8_t
    {
        return 0;
    }

    bool ServerCommandNPCShopDebugSlot::Execute(GamePlayer& player, int32_t page) const
    {
        const PlayerNPCShopComponent& playerNPCShopComponent = player.GetNPCShopComponent();

        if (!playerNPCShopComponent.HasShoppingNPC())
        {
            return false;
        }

        GameNPC& npc = playerNPCShopComponent.GetShoppingNPC();
        NPCItemShopComponent* npcItemShopComponent = npc.FindComponent<NPCItemShopComponent>();

        if (!npcItemShopComponent)
        {
            return false;
        }

        if (page >= npcItemShopComponent->GetItemStorageCount())
        {
            return false;
        }

        SUNLIGHT_LOG_INFO(_system.GetServiceLocator(),
            fmt::format("[{}]\n{}", GetName(), npcItemShopComponent->GetItemSlotDebugString(static_cast<int8_t>(page))));

        return true;
    }
}
