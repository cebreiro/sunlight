#pragma once
#include "sl/emulator/game/system/game_system.h"

namespace sunlight
{
    struct ZoneMessage;
    struct ZoneCommunityMessage;

    class SlPacketReader;
    class GameGroup;
    class GamePlayer;
    class GameItem;

    class IGroupMessageHandler;
}

namespace sunlight
{
    class PlayerGroupSystem final : public GameSystem
    {
    public:
        explicit PlayerGroupSystem(const ServiceLocator& serviceLocator);
        ~PlayerGroupSystem();

        void InitializeSubSystem(Stage& stage) override;
        bool Subscribe(Stage& stage) override;
        auto GetName() const -> std::string_view override;
        auto GetClassId() const -> game_system_id_type override;

        auto GetServiceLocator() const -> const ServiceLocator&;
        auto GetRandomEngine() -> std::mt19937&;

    public:
        void OnStageExit(GamePlayer& player);

        void AddStreetVendorGuest(int32_t groupId, GamePlayer& player);

        void ProcessHostExit(GameGroup& group, GamePlayer& host);
        void ProcessGuestExit(GameGroup& group, GamePlayer& guest);

        void ProcessTradeComplete(GameGroup& group, GamePlayer& host, GamePlayer& guest);
        void ProcessTradeFail(GameGroup& group, GamePlayer& player);

        void SpawnStoredItem(const GameGroup& group, GamePlayer& player, const GameItem& item, int32_t page, int32_t price, int32_t offset);

    private:
        void HandleStateProposition(const ZoneMessage& message);
        void HandleCreateGroup(const ZoneCommunityMessage& message);
        void HandleGroupMessage(const ZoneCommunityMessage& message);

    private:
        auto FindGroup(int32_t groupId) -> GameGroup*;
        auto FindGroup(int32_t groupId) const -> const GameGroup*;

        static auto CalculateStoredItemPosition(Eigen::Vector2f ownerPos, float ownerYaw, int64_t slot) -> std::pair<Eigen::Vector2f, float>;

    private:
        const ServiceLocator& _serviceLocator;

        int32_t _nextGroupId = 0;
        std::unordered_map<int32_t, UniquePtrNotNull<GameGroup>> _gameGroups;

        std::unordered_map<int32_t, UniquePtrNotNull<IGroupMessageHandler>> _itemMixMessageHandlers;
        std::unordered_map<int32_t, UniquePtrNotNull<IGroupMessageHandler>> _itemTradeMessageHandlers;
        std::unordered_map<int32_t, UniquePtrNotNull<IGroupMessageHandler>> _streetVendorMessageHandlers;

        std::mt19937 _mt;
    };
}
