#pragma once
#include "sl/data/asset_data.h"

namespace sunlight
{
    struct SodaDat
    {
        uint32_t packageDate = 0;
        uint32_t unk1 = 0;
        uint32_t assetDataCount = 0;
        uint32_t unk2 = 0;
        uint32_t unk3 = 0;
        uint32_t size = 0;

        std::unordered_map<uint32_t, AssetData> data;

        static auto CreateFrom(const std::filesystem::path& sodaDatFilePath) -> SodaDat;
    };
}
