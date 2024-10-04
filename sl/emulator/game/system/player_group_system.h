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

    public:
        void OnStageExit(GamePlayer& player);

        void AddStreetVendorGuest(int32_t groupId, GamePlayer& player);

    private:
        void HandleStateProposition(const ZoneMessage& message);
        void HandleCreateGroup(const ZoneCommunityMessage& message);
        void HandleGroupMessage(const ZoneCommunityMessage& message);

    private:
        void OnHostExit(GameGroup& group, GamePlayer& host);
        void OnGuestExit(GameGroup& group, GamePlayer& guest);

        bool HandleTradeMessage(GameGroup& group, GamePlayer& player, SlPacketReader& reader);
        void ProcessTradeFail(GameGroup& group, GamePlayer& player);

        bool HandleStreetVendorMessage(GameGroup& group, GamePlayer& player, SlPacketReader& reader);

    private:
        void SpawnStoredItem(const GameGroup& group, GamePlayer& player, const GameItem& item, int32_t page, int32_t price, int32_t offset);

    private:
        auto FindGroup(int32_t groupId) -> GameGroup*;
        auto FindGroup(int32_t groupId) const -> const GameGroup*;

        static auto CalculateStoredItemPosition(Eigen::Vector2f ownerPos, float ownerYaw, int64_t slot) -> std::pair<Eigen::Vector2f, float>;

    private:
        const ServiceLocator& _serviceLocator;

        int32_t _nextGroupId = 0;
        std::unordered_map<int32_t, UniquePtrNotNull<GameGroup>> _gameGroups;
    };
}
