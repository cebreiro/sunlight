#pragma once
/**********************************************************
*        AUTO GENERATED FILE BY sox_code_generator        *
*                  DO NOT EDIT THIS FILE                  *
**********************************************************/

#include "sl/data/sox/sox_table_interface.h"

namespace sunlight::sox
{
    struct EmotionList
    {
        explicit EmotionList(StreamReader<std::vector<char>::const_iterator>& reader);

        int32_t index = {};
        std::string name = {};
        int32_t group = {};
        int32_t mmotionId = {};
        int32_t fmotionId = {};
        int32_t mfxId = {};
        int32_t ffxId = {};
        int32_t page = {};
        int32_t levelRequire = {};
        int32_t itemRequire = {};
        int32_t itemConsume = {};
        int32_t emotionIndicate = {};
    };

    class EmotionListTable final : public ISoxTable, public std::enable_shared_from_this<EmotionListTable>
    {
    public:
        void LoadFromFile(const std::filesystem::path& path) override;

        auto Find(int32_t index) const -> const EmotionList*;
        auto Get() const -> const std::vector<EmotionList>&;

    private:
        std::unordered_map<int32_t, EmotionList*> _umap;
        std::vector<EmotionList> _vector;

    };
}
