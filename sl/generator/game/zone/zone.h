#pragma once
#include "sl/generator/game/zone/stage_exit_type.h"
#include "sl/generator/server/client/game_client_id.h"
#include "sl/generator/server/packet/zone_packet_c2s.h"
#include "sl/generator/server/packet/io/sl_packet_reader.h"
#include "sl/generator/service/database/dto/character.h"


namespace sunlight
{
    struct MapFile;

    class GameCommunityService;
    class GameRepositoryService;

    class Stage;
    class GameClient;
    class GamePlayer;
}

namespace sunlight
{
    class Zone final : public std::enable_shared_from_this<Zone>
    {
    public:
        Zone(const ServiceLocator& serviceLocator, execution::IExecutor& executor, int8_t worldId, int32_t id);
        ~Zone();

        void Initialize();

        void Start();
        void Shutdown();
        auto Join() -> Future<void>;

        auto SpawnPlayer(SharedPtrNotNull<GameClient> client, db::dto::Character dto) -> Future<bool>;
        auto LogoutPlayer(game_client_id_type id) -> Future<bool>;
        auto RemovePlayerByZoneChange(game_client_id_type id, int32_t destZoneId, int32_t destStage, float x, float y, float yaw) -> Future<bool>;
        auto ChangePlayerStage(game_client_id_type id, int32_t destStageId, int32_t destX, int32_t destY, std::optional<float> yaw) -> Future<bool>;

        void HandleClientDisconnect(game_client_id_type id);
        void HandleNetworkMessage(game_client_id_type id, ZonePacketC2S opcode, UniquePtrNotNull<SlPacketReader> reader);

    private:
        auto Update() -> Future<void>;

    public:
        auto FindStage(int32_t id) -> Stage*;
        auto FindStage(int32_t id) const -> const Stage*;
        auto FindPlayerStage(int64_t cid) -> Stage*;
        auto FindPlayerStage(int64_t cid) const -> const Stage*;
        auto FindClient(game_client_id_type id) -> GameClient*;
        auto FindClient(game_client_id_type id) const -> const GameClient*;

        auto GetServiceLocator() const -> const ServiceLocator&;
        auto GetStrand() -> Strand&;
        auto GetStrand() const -> const Strand&;
        auto GetWorldId() const -> int8_t;
        auto GetId() const -> int32_t;
        auto GetStages() const -> std::span<const UniquePtrNotNull<Stage>>;

    private:
        void HandleNetworkMessageImpl(game_client_id_type id, ZonePacketC2S opcode, UniquePtrNotNull<SlPacketReader> reader);

    private:
        ServiceLocator _serviceLocator;
        SharedPtrNotNull<Strand> _strand;

        int8_t _worldId = 0;
        int32_t _id = 0;
        PtrNotNull<const MapFile> _mapData = nullptr;

        std::atomic<bool> _shutdown = false;
        Future<void> _updateFuture;

        std::vector<UniquePtrNotNull<Stage>> _stages;
        std::unordered_map<game_client_id_type, PtrNotNull<Stage>> _playerStages;
        std::unordered_map<int64_t, PtrNotNull<Stage>> _playerCIdStageIndex;
        std::unordered_map<game_client_id_type, SharedPtrNotNull<GameClient>> _clients;

        SharedPtrNotNull<GameCommunityService> _gameCommunityService;
        SharedPtrNotNull<GameRepositoryService> _gameRepositoryService;
    };
}
