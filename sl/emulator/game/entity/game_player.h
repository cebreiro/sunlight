#pragma once
#include "sl/emulator/game/entity/game_entity.h"
#include "sl/emulator/server/client/game_client_id.h"

namespace sunlight::db::dto
{
    struct Character;
}

namespace sunlight
{
    class Stage;
    class GameClient;
    class GameDataProvideService;
    class GameEntityIdPool;

    class PlayerAppearanceComponent;
    class PlayerItemComponent;
    class PlayerJobComponent;
    class PlayerSkillComponent;
    class PlayerStatComponent;
    class SceneObjectComponent;
}

namespace sunlight
{
    class GamePlayer final : public GameEntity
    {
    public:
        static constexpr GameEntityType TYPE = GameEntityType::Player;

    public:
        GamePlayer(SharedPtrNotNull<GameClient> client, const db::dto::Character& dto,
            const GameDataProvideService& dataProvider, GameEntityIdPool& idPool);

        bool IsArmed() const;
        bool IsRunning() const;

        void Send(Buffer buffer);

        auto GetCid() const -> int64_t;
        auto GetAid() const -> int64_t;
        auto GetName() const -> const std::string&;
        auto GetZoneId() const -> int32_t;
        auto GetGmLevel() const -> int8_t;
        auto GetClientId() const -> game_client_id_type;
        auto GetStage() -> Stage&;
        auto GetStage() const -> const Stage&;

        void SetStage(Stage* stage);
        void SetArmed(bool value);
        void SetRunning(bool value);

    public:
        auto GetAppearanceComponent() -> PlayerAppearanceComponent&;
        auto GetAppearanceComponent() const -> const PlayerAppearanceComponent&;
        auto GetItemComponent() -> PlayerItemComponent&;
        auto GetItemComponent() const -> const PlayerItemComponent&;
        auto GetJobComponent() -> PlayerJobComponent&;
        auto GetJobComponent() const -> const PlayerJobComponent&;
        auto GetSkillComponent() -> PlayerSkillComponent&;
        auto GetSkillComponent() const -> const PlayerSkillComponent&;
        auto GetStatComponent() -> PlayerStatComponent&;
        auto GetStatComponent() const -> const PlayerStatComponent&;
        auto GetSceneObjectComponent() -> SceneObjectComponent&;
        auto GetSceneObjectComponent() const -> const SceneObjectComponent&;

    private:
        int64_t _cid = 0;
        int64_t _aid = 0;
        std::string _name;
        int32_t _zoneId = 0;
        int8_t _gmLevel = 0;
        bool _armed = false;
        bool _running = false;
        PtrNotNull<Stage> _stage = nullptr;

        SharedPtrNotNull<GameClient> _client;
    };
}
