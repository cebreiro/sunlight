#include "soda_dat.h"

#include "sl/lib/bzip/bzlib.h"
#include "sl/data/file_util.h"

namespace sunlight
{
    auto Decompress(std::vector<char>& buffer, char* result, uint32_t* size) -> int32_t
    {
        StreamReader reader(buffer.begin(), buffer.end());

        const int8_t a1 = buffer[0];

        [[maybe_unused]]
        const char* v5 = buffer.data();
        const bool v6 = a1 == 66;

        [[maybe_unused]]
        const char* v7 = buffer.data();

        [[maybe_unused]]
        uint32_t* v9 = nullptr;
        [[maybe_unused]]
        int32_t v10 = 0;

        if (v6 && buffer[1] == 90)
        {
            v9 = size;
            [[maybe_unused]] char* v11 = result;
        }
        else
        {
            assert(false);
        }

        return BZ2_bzBuffToBuffDecompress(result, v9, buffer.data(), static_cast<uint32_t>(buffer.size()), 0, 0);
    }

    auto SodaDat::CreateFrom(const std::filesystem::path& sodaDatFilePath) -> SodaDat
    {
        const std::vector<char> fileData = ReadBinaryFile(sodaDatFilePath);
        StreamReader reader(fileData.begin(), fileData.end());

        SodaDat soda;
        soda.packageDate = reader.Read<uint32_t>();
        soda.unk1 = reader.Read<uint32_t>();
        soda.assetDataCount = reader.Read<uint32_t>();
        soda.unk2 = reader.Read<uint32_t>();
        soda.unk3 = reader.Read<uint32_t>();
        soda.size = reader.Read<uint32_t>();

        // skip 528 byte
        std::array<char, 528> temp = {};
        reader.ReadBuffer(temp.data(), std::ssize(temp));

        const uint32_t v6 = soda.unk3 + soda.unk2;
        const uint32_t v7 = soda.unk3 + soda.unk2;

        const auto calculateSize = [](uint32_t v3, uint32_t v6, uint32_t v7) -> uint32_t
            {
                return 228 * v3 + 4 * (v6 + 16 * v7) + ((228 * v3 + 4 * (v6 + 16 * v7)) >> 3) + 256;
            };

        uint32_t decompressedSize = calculateSize(soda.assetDataCount, v6, v7);
        std::vector<char> decompressed(decompressedSize);

        if (soda.size > 0)
        {
            std::vector<char> compressed(soda.size);
            reader.ReadBuffer(compressed.data(), std::ssize(compressed));

            if (Decompress(compressed, decompressed.data(), &decompressedSize) < 0)
            {
                assert(false);

                throw std::runtime_error(fmt::format("fail to decompress soda.dat. path: {}",
                    sodaDatFilePath.string()));
            }
        }

        auto iter = decompressed.begin();

        for (uint32_t i = 0; i < soda.assetDataCount; ++i)
        {
            std::array<char, 228> buffer = {};
            std::copy_n(iter, std::ssize(buffer), buffer.begin());

            iter += std::ssize(buffer);

            StreamReader dataReader(buffer.begin(), buffer.end());

            const uint32_t id = dataReader.Read<uint32_t>();

            AssetData& data = soda.data[id];
            data.id = id;
            data.name = dataReader.ReadString();
            data.groupId = *reinterpret_cast<uint32_t*>(&buffer[196]);
            data.partId = *reinterpret_cast<uint32_t*>(&buffer[200]);
        }

        return soda;
    }
}
