#pragma once
/**********************************************************
*        AUTO GENERATED FILE BY sox_code_generator        *
*                  DO NOT EDIT THIS FILE                  *
**********************************************************/

#include "sl/data/sox/sox_table_interface.h"

namespace sunlight::sox
{
    struct MinigameSlmon
    {
        explicit MinigameSlmon(StreamReader<std::vector<char>::const_iterator>& reader);

        int32_t index = {};
        std::string unitname = {};
        int32_t modelid = {};
        int32_t unithp = {};
        int32_t boss = {};
        int32_t flying = {};
        int32_t moverule = {};
        int32_t team = {};
        std::string startpoint1 = {};
        std::string startpoint2 = {};
        std::string startpoint3 = {};
        std::string startpoint4 = {};
        std::string startpoint5 = {};
        int32_t attackbeatani = {};
        int32_t attackFxId = {};
        int32_t attackSound = {};
    };

    class MinigameSlmonTable final : public ISoxTable, public std::enable_shared_from_this<MinigameSlmonTable>
    {
    public:
        void LoadFromFile(const std::filesystem::path& path) override;

        auto Find(int32_t index) const -> const MinigameSlmon*;
        auto Get() const -> const std::vector<MinigameSlmon>&;

    private:
        std::unordered_map<int32_t, MinigameSlmon*> _umap;
        std::vector<MinigameSlmon> _vector;

    };
}
