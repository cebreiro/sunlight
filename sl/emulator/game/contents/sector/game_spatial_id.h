#pragma once

namespace sunlight
{
    namespace detail
    {
        class GameSpatialId
        {
        public:
            GameSpatialId() = default;
            GameSpatialId(int32_t x, int32_t y);

            auto GetX() const -> int32_t;
            auto GetY() const -> int32_t;

            bool operator<(GameSpatialId id) const;
            bool operator==(GameSpatialId other) const;
            bool operator!=(GameSpatialId other) const;

        private:
            int32_t _x = -1;
            int32_t _y = -1;
        };

        template <typename T>
        class GameSpatialIdT : public GameSpatialId
        {
        public:
            GameSpatialIdT(int32_t x, int32_t y)
                : GameSpatialId(x, y)
            {
            }
        };
    }

    using game_spatial_cell_id_type = detail::GameSpatialIdT<class GameSpatialCellIdTag>;
    using game_spatial_sector_id_type = detail::GameSpatialIdT<class GameSpatialSectorIdTag>;
}
