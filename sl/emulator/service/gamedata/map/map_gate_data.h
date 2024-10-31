#pragma once

namespace sunlight
{
    struct MapGateData
    {
        int32_t mapId = 0;
        int32_t linkId = 0;
        Eigen::Vector2f centerPosition = {};
        float yaw = 0.f;
    };
}
