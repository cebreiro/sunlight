#pragma once
#include <boost/container/static_vector.hpp>
#include "sl/emulator/game/contents/path_finding/tile_index.h"

namespace sunlight
{
    struct Tile
    {
        bool IsBlocked() const;
        auto GetPosition() const -> Eigen::Vector2f;

        TileIndex index = {};
        int8_t value = 0;

        boost::container::static_vector<PtrNotNull<Tile>, 8> connections;
    };
}
