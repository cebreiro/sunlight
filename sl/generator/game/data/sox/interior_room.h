#pragma once
/**********************************************************
*        AUTO GENERATED FILE BY sox_code_generator        *
*                  DO NOT EDIT THIS FILE                  *
**********************************************************/

#include "sl/data/sox/sox_table_interface.h"

namespace sunlight::sox
{
    struct InteriorRoom
    {
        explicit InteriorRoom(StreamReader<std::vector<char>::const_iterator>& reader);

        int32_t index = {};
        int32_t roomId = {};
        int32_t roomType = {};
        int32_t roomLv = {};
        int32_t roomCost = {};
        int32_t roomDesc = {};
        int32_t roomPreview = {};
    };

    class InteriorRoomTable final : public ISoxTable, public std::enable_shared_from_this<InteriorRoomTable>
    {
    public:
        void LoadFromFile(const std::filesystem::path& path) override;

        auto Find(int32_t index) const -> const InteriorRoom*;
        auto Get() const -> const std::vector<InteriorRoom>&;

    private:
        std::unordered_map<int32_t, InteriorRoom*> _umap;
        std::vector<InteriorRoom> _vector;

    };
}