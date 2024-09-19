#pragma once
#include "sl/emulator/server/client/game_client_id.h"
#include "sl/emulator/server/packet/zone_packet_c2s.h"
#include "sl/emulator/server/packet/io/sl_packet_reader.h"
#include "sl/emulator/service/database/dto/character.h"

namespace sunlight
{
    struct MapFile;

    class Stage;
    class GameClient;
    class GamePlayer;
}

namespace sunlight
{
    class Zone final : public std::enable_shared_from_this<Zone>
    {
    public:
        Zone(const ServiceLocator& serviceLocator, execution::IExecutor& executor, int32_t id);
        ~Zone();

        void Start();
        void Shutdown();
        void Join();

        auto SpawnPlayer(SharedPtrNotNull<GameClient> client, db::dto::Character dto) -> Future<bool>;
        void HandleNetworkMessage(game_client_id_type id, ZonePacketC2S opcode, UniquePtrNotNull<SlPacketReader> reader);

    private:
        auto Update() -> Future<void>;

    public:
        auto FindStage(int32_t id) -> Stage*;
        auto FindStage(int32_t id) const -> const Stage*;

        auto GetServiceLocator() const -> const ServiceLocator&;
        auto GetStrand() -> Strand&;
        auto GetStrand() const -> const Strand&;
        auto GetId() const -> int32_t;

    private:
        void HandleNetworkMessageImpl(game_client_id_type id, ZonePacketC2S opcode, UniquePtrNotNull<SlPacketReader> reader);

    private:
        ServiceLocator _serviceLocator;
        SharedPtrNotNull<Strand> _strand;

        int32_t _id = 0;
        PtrNotNull<const MapFile> _mapData = nullptr;

        std::atomic<bool> _shutdown = false;
        Future<void> _updateFuture;

        std::vector<UniquePtrNotNull<Stage>> _stages;
        std::unordered_map<game_client_id_type, PtrNotNull<Stage>> _playerStages;
    };
}
