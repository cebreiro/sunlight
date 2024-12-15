#include "entity_scan_system.h"

#include "shared/collision/intersect.h"
#include "shared/collision/shape/circle.h"
#include "sl/generator/game/component/scene_object_component.h"
#include "sl/generator/game/contents/sector/game_spatial_sector.h"
#include "sl/generator/game/system/entity_view_range_system.h"
#include "sl/generator/game/zone/stage.h"

namespace sunlight
{
    void EntityScanSystem::InitializeSubSystem(Stage& stage)
    {
        Add(stage.Get<EntityViewRangeSystem>());
    }

    bool EntityScanSystem::Subscribe(Stage& stage)
    {
        (void)stage;

        return true;
    }

    bool EntityScanSystem::ShouldUpdate() const
    {
        return true;
    }

    void EntityScanSystem::Update()
    {
        ProcessScan();
    }

    auto EntityScanSystem::GetName() const -> std::string_view
    {
        return "entity_scan_system";
    }

    auto EntityScanSystem::GetClassId() const -> game_system_id_type
    {
        return GameSystem::GetClassId<EntityScanSystem>();
    }

    auto EntityScanSystem::ScanPlayerAsync(std::vector<std::pair<game_entity_id_type, float>>& result, const GameMonster& monster, float range) -> Future<void>
    {
        Promise<void> promise;
        Future<void> future = promise.GetFuture();

        _scanRequests.emplace_back(ScanRequest{
            .type = GameEntityType::Player,
            .center = monster.GetSceneObjectComponent().GetPosition(),
            .range = range,
            .result = &result,
            .promise = std::move(promise),
            });

        return future;
    }

    void EntityScanSystem::ScanMonsterAttackTarget(std::vector<game_entity_id_type>& result, const Eigen::Vector2f& position, float range)
    {
        GameSpatialSector& sector = Get<EntityViewRangeSystem>().GetSector(position);
        const auto sectorId = sector.GetId();

        const collision::Circle circle(position, range);

        for (const GameSpatialCell& cell : sector.GetCells())
        {
            // TODO: pet
            
            if (cell.Empty(GameEntityType::Player))
            {
                continue;
            }

            if (sectorId != cell.GetId())
            {
                if (CalculateCellMinDistanceSq(sector, cell, position) >  (range * range))
                {
                    continue;
                }
            }

            for (const GameEntity& target : cell.GetEntities(GameEntityType::Player))
            {
                if (IsIntersection(target, circle))
                {
                    result.emplace_back(target.GetId());
                }
            }
        }
    }

    void EntityScanSystem::ProcessScan()
    {
        if (_scanRequests.empty())
        {
            return;
        }

        EntityViewRangeSystem& entityViewRangeSystem = Get<EntityViewRangeSystem>();

        for (ScanRequest& scanRequest : _scanRequests)
        {
            scanRequest.sector = &entityViewRangeSystem.GetSector(scanRequest.center);
        }

        const auto comparer = [](const ScanRequest& lhs, const ScanRequest& rhs) -> bool
            {
                return lhs.sector->GetId() < rhs.sector->GetId();
            };

        std::ranges::sort(_scanRequests, comparer);
        _scanRequestSubRanges.clear();

        auto iter = _scanRequests.begin();
        while (iter != _scanRequests.end())
        {
            const auto [first, last] = std::ranges::equal_range(iter, _scanRequests.end(), *iter, comparer);
            _scanRequestSubRanges.emplace_back(first, last);

            iter = last;
        }
        
        for (const auto& subRange : _scanRequestSubRanges)
        {
            GameSpatialSector* sector = subRange.begin()->sector;
            const auto sectorId = sector->GetId();

            for (const GameSpatialCell& cell : sector->GetCells())
            {
                for (ScanRequest& scanRequest : subRange)
                {
                    if (cell.Empty(scanRequest.type))
                    {
                        continue;
                    }

                    if (const auto cellId = cell.GetId();
                        cellId != sectorId)
                    {
                        const float distanceSq = CalculateCellMinDistanceSq(*sector, cell, scanRequest.center);
                        if (distanceSq > scanRequest.range * scanRequest.range)
                        {
                            continue;
                        }
                    }

                    const collision::Circle scanCollision(scanRequest.center, scanRequest.range);

                    for (const GameEntity& target : cell.GetEntities(scanRequest.type))
                    {
                        if (IsIntersection(target, scanCollision))
                        {
                            scanRequest.result->emplace_back(target.GetId(), (target.GetComponent<SceneObjectComponent>().GetPosition() - scanRequest.center).norm());
                        }
                    }
                }
            }
        }

        for (ScanRequest& scanRequest : _scanRequests)
        {
            if (scanRequest.promise.has_value())
            {
                scanRequest.promise->Set();
            }
        }

        _scanRequests.clear();
    }

    auto EntityScanSystem::CalculateCellMinDistanceSq(const GameSpatialSector& sector, const GameSpatialCell& cellInSector, const Eigen::Vector2f& posInSector) -> float
    {
        const auto sectorId = sector.GetId();
        const auto cellId = cellInSector.GetId();
        assert(cellId != sectorId);

        const GameSpatialMBR& mbr = cellInSector.GetMBR();

        float diffX = 0.f;
        float diffY = 0.f;

        if (cellId.GetX() != sectorId.GetX())
        {
            diffX = sectorId.GetX() < cellId.GetX()
                ? mbr.GetMin().x() - posInSector.x()
                : posInSector.x() - mbr.GetMax().x();
            assert(diffX >= 0.f);
        }

        if (cellId.GetY() != sectorId.GetY())
        {
            diffY = sectorId.GetY() < cellId.GetY()
                ? mbr.GetMin().y() - posInSector.y()
                : posInSector.y() - mbr.GetMax().y();
            assert(diffY >= 0.f);
        }

        return (diffX * diffX) + (diffY * diffY);
    }

    bool EntityScanSystem::IsIntersection(const GameEntity& target, const collision::Circle& circle)
    {
        const SceneObjectComponent& sceneObjectComponent = target.GetComponent<SceneObjectComponent>();
        const Eigen::Vector2f& position = sceneObjectComponent.GetPosition();
        const collision::Circle targetCollision(position, static_cast<float>(sceneObjectComponent.GetBodySize()));

        return Intersect(circle, targetCollision);
    }
}
