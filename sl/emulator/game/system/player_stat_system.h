#pragma once
#include "sl/emulator/game/contants/stat/stat_value.h"
#include "sl/emulator/game/system/game_system.h"

namespace sunlight::sox
{
    struct JobReference;
}

namespace sunlight
{
    struct ZoneMessage;

    class GameItem;
    class GamePlayer;
    class PlayerStatComponent;
}

namespace sunlight
{
    class PlayerStatSystem final : public GameSystem
    {
    public:
        explicit PlayerStatSystem(const ServiceLocator& serviceLocator);

        void InitializeSubSystem(Stage& stage) override;
        bool Subscribe(Stage& stage) override;

        void GainCharacterExp(GamePlayer& player, int32_t exp);

        auto GetName() const -> std::string_view override;
        auto GetClassId() const -> game_system_id_type override;

    public:
        void AddItemStat(GamePlayer& player, const GameItem& item);
        void RemoveItemStat(GamePlayer& player, const GameItem& item);

    public:
        void OnInitialize(GamePlayer& player);
        void OnLocalActivate(GamePlayer& player);
        void OnStatPointUse(const ZoneMessage& message);

    private:
        static auto CalculateJobMaxHP(const sox::JobReference& data, int32_t jobLevel,
            const PlayerStatComponent& statComponent) -> StatValue;
        static auto CalculateJobMaxSP(const sox::JobReference& data, int32_t jobLevel,
            const PlayerStatComponent& statComponent) -> StatValue;
        static auto CalculateJobRecoveryHP(const sox::JobReference& data,
            const PlayerStatComponent& statComponent) -> StatValue;
        static auto CalculateJobRecoverySP(const sox::JobReference& data,
            const PlayerStatComponent& statComponent) -> StatValue;

    private:
        const ServiceLocator& _serviceLocator;
    };
}
