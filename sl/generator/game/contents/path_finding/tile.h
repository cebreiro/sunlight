#pragma once
#include <boost/container/static_vector.hpp>
#include "sl/generator/game/contents/path_finding/tile_index.h"

namespace sunlight
{
    struct Tile
    {
        auto GetPosition() const -> Eigen::Vector2f;

        TileIndex index = {};
        bool blocked = false;

        boost::container::static_vector<PtrNotNull<Tile>, 8> connections;
    };
}
