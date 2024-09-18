#pragma once
#include "sl/emulator/game/entity/game_entity_id_type.h"
#include "sl/emulator/game/system/game_system.h"

namespace sunlight
{
    class GameEntity;
}

namespace sunlight
{
    class EntityViewRangeSystem final : public GameSystem
    {
    public:
        void InitializeSubSystem(Stage& stage) override;
        bool Subscribe(Stage& stage) override;
        auto GetName() const -> std::string_view override;
        auto GetClassId() const -> game_system_id_type override;

    public:
        bool Contains(game_entity_id_type id) const;

        void Add(PtrNotNull<GameEntity> entity);
        void Remove(PtrNotNull<GameEntity> entity);

        void Update(game_entity_id_type id, const Eigen::Vector3f& position);

    public:
        void Broadcast(game_entity_id_type centerEntityId, Buffer buffer, bool includeCenter);
    };
}
