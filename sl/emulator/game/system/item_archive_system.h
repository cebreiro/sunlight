#pragma once
#include "sl/data/sox/item/equipment_type.h"
#include "sl/emulator/game/contants/item/equipment_position.h"
#include "sl/emulator/game/entity/game_entity_id_type.h"
#include "sl/emulator/game/message/zone_message_type.h"
#include "sl/emulator/game/system/game_system.h"

namespace sunlight
{
    class GamePlayer;
    struct ZoneMessage;
}

namespace sunlight
{
    class ItemArchiveSystem final : public GameSystem
    {
    public:
        explicit ItemArchiveSystem(const ServiceLocator& serviceLocator);

        void InitializeSubSystem(Stage& stage) override;
        bool Subscribe(Stage& stage) override;

        auto GetName() const -> std::string_view override;
        auto GetClassId() const -> game_system_id_type override;

    public:
        static bool IsValid(EquipmentPosition position, sox::EquipmentType soxType);

    private:
        void HandleMessage(const ZoneMessage& message);

        bool HandleLiftItem(GamePlayer& player, game_entity_id_type pick, int32_t quantity);
        bool HandleLowerItem(GamePlayer& player, game_entity_id_type lowerItemId, game_entity_id_type liftItemId,
            int8_t page, int8_t x, int8_t y);

        bool HandleLiftEquipment(GamePlayer& player, EquipmentPosition position);
        bool HandleLowerEquipment(GamePlayer& player, game_entity_id_type equipItemId, EquipmentPosition position);

    private:
        void LogHandleItemError(const char* func, const std::string& message) const;

    private:
        const ServiceLocator& _serviceLocator;
    };
}
