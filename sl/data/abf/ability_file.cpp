#include "ability_file.h"

#include <boost/lexical_cast.hpp>

#include "sl/data/file_util.h"

namespace
{
    const char* abf_version_1_0 = "SL Ability Filie v1.0";
    const char* abf_version_1_1 = "SL Ability Filie v1.1";
    const char* abf_version_1_2 = "SL Ability Filie v1.2";
}

namespace sunlight
{
    auto AbilityFile::CreateFrom(const std::filesystem::path& filePath) -> AbilityFile
    {
        const std::vector<char> fileData = ReadBinaryFile(filePath);

        StreamReader reader(fileData.begin(), fileData.end());

        AbilityFile abf;
        abf.name = filePath.filename().stem().string();

        std::array<char, 296> header = {};
        reader.ReadBuffer(header.data(), AbilityFile::HEADER_SIZE);

        if (::strcmp(header.data(), abf_version_1_0) != 0)
        {
            if (!::strcmp(header.data(), abf_version_1_1))
            {
                abf.version = AbilityFileVersion::V1_1;
            }
            else if (!::strcmp(header.data(), abf_version_1_2))
            {
                abf.version = AbilityFileVersion::V1_2;
                abf.version1_2 = true;
            }
            else
            {
                throw std::runtime_error(fmt::format("invalid ability file version. path: {}", filePath.string()));
            }

            abf.unk1 = reader.Read<float>();
            reader.ReadBuffer(reinterpret_cast<char*>(abf.unk2.data()), 16);
        }
        else
        {
            abf.version = AbilityFileVersion::V1_0;
        }

        const int32_t routineCount = *reinterpret_cast<int32_t*>(header.data() + 32);
        abf.routines.resize(routineCount);

        for (AbilityRoutine& routine : abf.routines)
        {
            const std::string& str = reader.ReadString(4);

            try
            {
                routine.weaponClass = boost::lexical_cast<int32_t>(str);
            }
            catch (const std::exception& e)
            {
                std::cout << e.what() << '\n';
            }
            
            reader.ReadBuffer(reinterpret_cast<char*>(routine.data.data()), 28);
            routine.time = reader.Read<int32_t>();

            const int32_t propertyCount = reader.Read<int32_t>();
            routine.properties.resize(propertyCount);

            for (AbilityProperty& property : routine.properties)
            {
                const int32_t valueCount = reader.Read<int32_t>();
                property.unk1 = reader.Read<int32_t>();;

                property.values.resize(valueCount);

                for (AbilityValue& value : property.values)
                {
                    value.type = reader.Read<int32_t>();
                    value.unk1 = reader.Read<int32_t>();
                    value.begin = reader.Read<int32_t>();
                    value.end = reader.Read<int32_t>();
                    value.unk4 = reader.Read<int32_t>();
                    value.assetName = reader.ReadString(64);
                    value.unk5 = reader.Read<int32_t>();

                    value.otherUnk1 = reader.Read<int32_t>();
                    value.otherUnk2 = reader.Read<int32_t>();
                    value.otherUnk3 = reader.Read<float>();
                    value.damageCount = reader.Read<int32_t>();
                    reader.ReadBuffer(value.buffer.data(), std::ssize(value.buffer));
                }
            }
        }

        return abf;
    }
}
