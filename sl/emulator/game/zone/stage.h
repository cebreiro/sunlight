#pragma once
#include "sl/emulator/game/message/character_message_type.h"
#include "sl/emulator/game/message/zone_message_type.h"
#include "sl/emulator/server/client/game_client_id.h"
#include "sl/emulator/server/packet/zone_packet_c2s.h"
#include "sl/emulator/game/system/game_system.h"
#include "sl/emulator/server/packet/io/sl_packet_reader.h"

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

        void SpawnPlayer(SharedPtrNotNull<GamePlayer> player);

        bool AddSubscriber(ZonePacketC2S type, const std::function<void(const ZoneRequest&)>& subscriber);
        bool AddSubscriber(ZoneMessageType type, const std::function<void(const ZoneMessage&)>& subscriber);
        bool AddSubscriber(ZoneMessageType type, const std::function<void(const ZoneCommunityMessage&)>& subscriber);
        bool AddSubscriber(CharacterMessageType type, const std::function<void(const CharacterMessage&)>& subscriber);

        auto GetId() const -> int32_t;
        auto GetName() const -> const std::string&;
        auto GetServiceLocator() const -> const ServiceLocator&;

        template <typename T> requires std::derived_from<T, GameSystem>
        auto Get() -> T&;

    private:
        template <typename T> requires std::derived_from<T, GameSystem>
        bool Add(SharedPtrNotNull<T> system);

        void InitializeSystem();
        void InitializeNPC(const std::vector<MapProp>& props);
        void InitializeNPC(const std::vector<MapTerrainProp>& props);

        bool Publish(const ZoneRequest& request);
        bool Publish(const ZoneMessage& message);
        bool Publish(const ZoneCommunityMessage& message);
        bool Publish(const CharacterMessage& message);

    private:
        static auto ExtractPositionAndYaw(const Eigen::Matrix4f& matrix) -> std::pair<Eigen::Vector3f, float>;

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
}
