#include "entity_view_range_system.h"

namespace sunlight
{
    void EntityViewRangeSystem::InitializeSubSystem(Stage& stage)
    {
        (void)stage;
    }

    bool EntityViewRangeSystem::Subscribe(Stage& stage)
    {
        return true;
    }

    auto EntityViewRangeSystem::GetName() const -> std::string_view
    {
        return "entity_view_range_system";
    }

    auto EntityViewRangeSystem::GetClassId() const -> game_system_id_type
    {
        return GameSystem::GetClassId<EntityViewRangeSystem>();;
    }

    bool EntityViewRangeSystem::Contains(game_entity_id_type id) const
    {
        (void)id;

        return true;
    }

    void EntityViewRangeSystem::Add(PtrNotNull<GameEntity> entity)
    {
        (void)entity;
    }

    void EntityViewRangeSystem::Remove(PtrNotNull<GameEntity> entity)
    {
        (void)entity;
    }

    void EntityViewRangeSystem::Update(game_entity_id_type id, const Eigen::Vector3f& position)
    {
        (void)id;
        (void)position;
    }

    void EntityViewRangeSystem::Broadcast(game_entity_id_type centerEntityId, Buffer buffer, bool includeCenter)
    {
        (void)centerEntityId;
        (void)buffer;
        (void)includeCenter;
    }
}
