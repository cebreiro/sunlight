#include "entity_intialization_system.h"

#include "sl/emulator/game/entity/game_player.h"
#include "sl/emulator/game/message/zone_message.h"
#include "sl/emulator/game/message/creator/game_player_message_creator.h"
#include "sl/emulator/game/zone/stage.h"

namespace sunlight
{
    bool EntityInitializationSystem::Subscribe(Stage& stage)
    {
        if (!stage.AddSubscriber(ZoneMessageType::REQUEST_ALL_STATE,
            std::bind_front(&EntityInitializationSystem::HandlePlayerAllState, this)))
        {
            return false;
        }

        if (!stage.AddSubscriber(ZoneMessageType::LOCAL_ACTIVATED,
            std::bind_front(&EntityInitializationSystem::HandlePlayerActivate, this)))
        {
            return false;
        }

        return true;
    }

    auto EntityInitializationSystem::GetClassId() const -> game_system_id_type
    {
        return GameSystem::GetClassId<EntityInitializationSystem>();
    }

    void EntityInitializationSystem::HandlePlayerAllState(const ZoneMessage& message)
    {
        message.player.Send(GamePlayerMessageCreator::CreateAllState(message.player));
    }

    void EntityInitializationSystem::HandlePlayerActivate(const ZoneMessage& message)
    {
        message.player.SetActive(true);
    }
}
