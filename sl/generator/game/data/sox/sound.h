#pragma once
/**********************************************************
*        AUTO GENERATED FILE BY sox_code_generator        *
*                  DO NOT EDIT THIS FILE                  *
**********************************************************/

#include "sl/data/sox/sox_table_interface.h"

namespace sunlight::sox
{
    struct Sound
    {
        explicit Sound(StreamReader<std::vector<char>::const_iterator>& reader);

        int32_t index = {};
        int32_t soundKey1Time = {};
        int32_t soundKey1Wav = {};
        int32_t soundKey2Time = {};
        int32_t soundKey2Wav = {};
        int32_t soundKey3Time = {};
        int32_t soundKey3Wav = {};
        int32_t soundKey4Time = {};
        int32_t soundKey4Wav = {};
        int32_t soundKey5Time = {};
        int32_t soundKey5Wav = {};
        int32_t soundKey6Time = {};
        int32_t soundKey6Wav = {};
        int32_t soundKey7Time = {};
        int32_t soundKey7Wav = {};
        int32_t soundKey8Time = {};
        int32_t soundKey8Wav = {};
    };

    class SoundTable final : public ISoxTable, public std::enable_shared_from_this<SoundTable>
    {
    public:
        void LoadFromFile(const std::filesystem::path& path) override;

        auto Find(int32_t index) const -> const Sound*;
        auto Get() const -> const std::vector<Sound>&;

    private:
        std::unordered_map<int32_t, Sound*> _umap;
        std::vector<Sound> _vector;

    };
}
