#pragma once
#include "sl/generator/game/entity/game_entity.h"
#include "sl/generator/server/client/game_client_id.h"

namespace sunlight::db::dto
{
    struct Character;
}

namespace sunlight
{
    class NPCTalkBox;
    class EventScript;
    class Stage;
    class GameClient;
    class GameNPC;
    class GameDataProvideService;

    class PlayerAppearanceComponent;
    class PlayerItemComponent;
    class PlayerJobComponent;
    class PlayerSkillComponent;
    class PlayerStatComponent;
    class PlayerNPCScriptComponent;
    class PlayerQuestComponent;
    class PlayerNPCShopComponent;
    class PlayerProfileComponent;
    class PlayerGroupComponent;
    class PlayerPartyComponent;
    class PlayerDebugComponent;
    class SceneObjectComponent;
    class EntityMovementComponent;
    class EntityStateComponent;
    class EntityStatusEffectComponent;
    class EntityPassiveEffectComponent;
    class EntityImmuneComponent;
}

namespace sunlight
{
    class GamePlayer final : public GameEntity
    {
    public:
        static constexpr GameEntityType TYPE = GameEntityType::Player;

    public:
        GamePlayer(SharedPtrNotNull<GameClient> client, const db::dto::Character& dto,
            const GameDataProvideService& dataProvider, GameEntityIdPublisher& idPublisher);

        bool HasDeferred() const;

        void Defer(Buffer buffer);
        void FlushDeferred();

        void Send(Buffer buffer);

    public:
        void Notice(const std::string& message);
        void Talk(const GameNPC& npc, const NPCTalkBox& talkBox);
        void Show(const EventScript& eventScript);

    public:
        bool IsArmed() const;
        bool IsRunning() const;
        bool IsDead() const;

        auto GetCId() const -> int64_t;
        auto GetAId() const -> int64_t;
        auto GetName() const -> const std::string&;
        auto GetZoneId() const -> int32_t;
        auto GetGmLevel() const -> int8_t;
        auto GetClient() const -> GameClient&;
        auto GetClientId() const -> game_client_id_type;

        void SetArmed(bool value);
        void SetRunning(bool value);
        void SetDead(bool value);

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
        auto GetMovementComponent() -> EntityMovementComponent&;
        auto GetMovementComponent() const -> const EntityMovementComponent&;
        auto GetStateComponent() -> EntityStateComponent&;
        auto GetStateComponent() const -> const EntityStateComponent&;
        auto GetNPCScriptComponent() -> PlayerNPCScriptComponent&;
        auto GetNPCScriptComponent() const -> const PlayerNPCScriptComponent&;
        auto GetQuestComponent() -> PlayerQuestComponent&;
        auto GetQuestComponent() const -> const PlayerQuestComponent&;
        auto GetNPCShopComponent() -> PlayerNPCShopComponent&;
        auto GetNPCShopComponent() const -> const PlayerNPCShopComponent&;
        auto GetProfileComponent() -> PlayerProfileComponent&;
        auto GetProfileComponent() const -> const PlayerProfileComponent&;
        auto GetGroupComponent() -> PlayerGroupComponent&;
        auto GetGroupComponent() const -> const PlayerGroupComponent&;
        auto GetPartyComponent() -> PlayerPartyComponent&;
        auto GetPartyComponent() const -> const PlayerPartyComponent&;
        auto GetStatusEffectComponent() -> EntityStatusEffectComponent&;
        auto GetStatusEffectComponent() const -> const EntityStatusEffectComponent&;
        auto GetDebugComponent() -> PlayerDebugComponent&;
        auto GetDebugComponent() const -> const PlayerDebugComponent&;
        auto GetPassiveEffectComponent() -> EntityPassiveEffectComponent&;
        auto GetPassiveEffectComponent() const -> const EntityPassiveEffectComponent&;
        auto GetImmuneComponent() -> EntityImmuneComponent&;
        auto GetImmuneComponent() const -> const EntityImmuneComponent&;

    private:
        int64_t _cid = 0;
        int64_t _aid = 0;
        std::string _name;
        int32_t _zoneId = 0;
        int8_t _gmLevel = 0;
        bool _armed = false;
        bool _running = false;
        bool _dead = false;

        SharedPtrNotNull<GameClient> _client;

        std::vector<Buffer> _deferred;
    };
}
