#pragma once
/**********************************************************
*        AUTO GENERATED FILE BY sox_code_generator        *
*                  DO NOT EDIT THIS FILE                  *
**********************************************************/

#include "sl/data/sox/sox_table_interface.h"

namespace sunlight::sox
{
    struct InteriorFurniture
    {
        explicit InteriorFurniture(StreamReader<std::vector<char>::const_iterator>& reader);

        int32_t index = {};
        int32_t furnitureId = {};
        int32_t furnitureType = {};
        int32_t furnitureLv = {};
        int32_t furnitureCost = {};
        int32_t furnitureDesc = {};
        int32_t furniturePreview = {};
        int32_t furnitureAnitype = {};
    };

    class InteriorFurnitureTable final : public ISoxTable, public std::enable_shared_from_this<InteriorFurnitureTable>
    {
    public:
        void LoadFromFile(const std::filesystem::path& path) override;

        auto Find(int32_t index) const -> const InteriorFurniture*;
        auto Get() const -> const std::vector<InteriorFurniture>&;

    private:
        std::unordered_map<int32_t, InteriorFurniture*> _umap;
        std::vector<InteriorFurniture> _vector;

    };
}
