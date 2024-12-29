#pragma once
#include "sl/data/map/map_event_object.h"
#include "sl/generator/game/message/character_message_type.h"
#include "sl/generator/game/message/zone_message_type.h"
#include "sl/generator/game/zone/stage_enter_type.h"
#include "sl/generator/game/zone/stage_exit_type.h"
#include "sl/generator/server/client/game_client_id.h"
#include "sl/generator/server/packet/zone_packet_c2s.h"
#include "sl/generator/game/system/game_system.h"
#include "sl/generator/game/time/game_time.h"
#include "sl/generator/server/packet/io/sl_packet_reader.h"

namespace sunlight
{
    struct MapProp;
    struct MapTerrainProp;
    struct MapStage;
    struct CharacterMessage;
    struct ZoneRequest;
    struct ZoneMessage;
    struct ZoneCommunityMessage;

    class GameEntity;
    class GamePlayer;
    class GameItem;
    class ZoneMessageHooker;
}

namespace sunlight
{
    class Stage
    {
    public:
        Stage(const Stage& other) = delete;
        Stage(Stage&& other) noexcept = delete;
        Stage& operator=(const Stage& other) = delete;
        Stage& operator=(Stage&& other) noexcept = delete;

    public:
        Stage(const ServiceLocator& serviceLocator, int32_t zoneId, const MapStage& stageData);
        ~Stage();

        void Update();

        void HandleNetworkMessage(game_client_id_type id, ZonePacketC2S opcode, UniquePtrNotNull<SlPacketReader> reader);

        void SpawnPlayer(SharedPtrNotNull<GamePlayer> player, StageEnterType enterType);
        auto DespawnPlayer(game_client_id_type clientId, StageExitType exitType) -> Future<std::shared_ptr<GamePlayer>>;

        bool AddSubscriber(ZonePacketC2S type, const std::function<void(const ZoneRequest&)>& subscriber);
        bool AddSubscriber(ZoneMessageType type, const std::function<void(const ZoneMessage&)>& subscriber);
        bool AddSubscriber(ZoneMessageType type, const std::function<void(const ZoneCommunityMessage&)>& subscriber);
        bool AddSubscriber(CharacterMessageType type, const std::function<void(const CharacterMessage&)>& subscriber);

        auto GetId() const -> int32_t;
        auto GetName() const -> const std::string&;
        auto GetServiceLocator() const -> const ServiceLocator&;

        template <typename T> requires std::derived_from<T, GameSystem>
        auto Get() -> T&;

        template <typename T> requires std::derived_from<T, GameSystem>
        auto Find() -> T*;

    private:
        template <typename T> requires std::derived_from<T, GameSystem>
        bool Add(SharedPtrNotNull<T> system);

        void InitializeSystem();
        void InitializeNPC(const std::vector<MapProp>& props);
        void InitializeNPC(const std::vector<MapTerrainProp>& props);
        void InitializeEventObject(const std::vector<MapEventObjectV3>& events);
        void InitializeEventObject(const std::vector<MapEventObjectV5>& events);

        bool Publish(const ZoneRequest& request);
        bool Publish(const ZoneMessage& message);
        bool Publish(const ZoneCommunityMessage& message);
        bool Publish(const CharacterMessage& message);

    private:
        const ServiceLocator& _serviceLocator;
        int32_t _zoneId = 0;
        const MapStage& _stageData;
        std::string _name;

        UniquePtrNotNull<ZoneMessageHooker> _zoneMessageHooker;

        std::unordered_map<game_system_id_type, SharedPtrNotNull<GameSystem>> _systems;
        std::vector<PtrNotNull<GameSystem>> _updateSystems;

        std::unordered_map<ZonePacketC2S, std::function<void(const ZoneRequest&)>> _zoneRequestSubscribers;
        std::unordered_map<ZoneMessageType, std::function<void(const ZoneMessage&)>> _zoneMessageSubscribers;
        std::unordered_map<ZoneMessageType, std::function<void(const ZoneCommunityMessage&)>> _zoneCommunityMessageSubscribers;
        std::unordered_map<CharacterMessageType, std::function<void(const CharacterMessage&)>> _characterMessageSubscriber;

        std::unordered_map<game_client_id_type, SharedPtrNotNull<GamePlayer>> _players;
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

    template <typename T> requires std::derived_from<T, GameSystem>
    auto Stage::Find() -> T*
    {
        const auto& id = GameSystem::GetClassId<T>();

        const auto iter = _systems.find(id);
        if (iter == _systems.end())
        {
            return nullptr;
        }

        T* result = static_cast<T*>(iter->second.get());
        assert(result == dynamic_cast<T*>(iter->second.get()));

        return result;
    }
}
