#pragma once
#include <boost/unordered/unordered_flat_map.hpp>
#include "sl/emulator/game/entity/game_entity_id_type.h"
#include "sl/emulator/game/entity/game_entity_type.h"
#include "sl/emulator/game/message/zone_message_type.h"
#include "sl/emulator/server/client/game_client_id.h"
#include "sl/emulator/server/packet/zone_packet_c2s.h"
#include "sl/emulator/game/system/game_system.h"
#include "sl/emulator/server/packet/io/sl_packet_reader.h"

namespace sunlight
{
    class GameEntity;
    class GamePlayer;
    struct ZoneMessage;
    struct ZoneCommunityMessage;
}

namespace sunlight
{
    class Stage
    {
    public:
        Stage(const ServiceLocator& serviceLocator, int32_t zoneId, int32_t stageId);
        ~Stage();

        void HandleNetworkMessage(game_client_id_type id, ZonePacketC2S opcode, UniquePtrNotNull<SlPacketReader> reader);

        void SpawnPlayer(SharedPtrNotNull<GamePlayer> player);

        bool AddSubscriber(ZoneMessageType type, const std::function<void(const ZoneMessage&)>& subscriber);
        bool AddSubscriber(ZoneMessageType type, const std::function<void(const ZoneCommunityMessage&)>& subscriber);

        auto GetId() const -> int32_t;
        auto GetName() const -> const std::string&;

        template <typename T> requires std::derived_from<T, GameSystem>
        auto Get() -> T&;

    private:
        template <typename T> requires std::derived_from<T, GameSystem>
        bool Add(SharedPtrNotNull<T> system);

        void InitializeSystem();

        bool Publish(const ZoneMessage& message);
        bool Publish(const ZoneCommunityMessage& message);

    private:
        const ServiceLocator& _serviceLocator;
        int32_t _id = 0;
        int32_t _zoneId = 0;
        std::string _name;

        std::unordered_map<game_system_id_type, SharedPtrNotNull<GameSystem>> _systems;
        std::unordered_map<ZoneMessageType, std::function<void(const ZoneMessage&)>> _zoneMessageSubscribers;
        std::unordered_map<ZoneMessageType, std::function<void(const ZoneCommunityMessage&)>> _zoneCommunityMessageSubscribers;

        std::unordered_map<game_client_id_type, SharedPtrNotNull<GamePlayer>> _players;
        std::unordered_map<GameEntityType,
            boost::unordered::unordered_flat_map<game_entity_id_type, SharedPtrNotNull<GameEntity>>> _entities;
    };

    template <typename T> requires std::derived_from<T, GameSystem>
    auto Stage::Get() -> T&
    {
        const auto& id = GameSystem::GetClassId<T>();

        const auto iter = _systems.find(id);
        if (iter == _systems.end())
        {
            assert(false);

            throw std::runtime_error("fail to find system");
        }

        T* result = static_cast<T*>(iter->second.get());
        assert(result == dynamic_cast<T*>(iter->second.get()));

        return *result;
    }
}
