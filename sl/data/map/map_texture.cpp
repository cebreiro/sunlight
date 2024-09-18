#include "map_texture.h"

#include "sl/data/map/map_stream_utility.h"

namespace sunlight
{
    MapTexture::MapTexture(StreamReader<std::vector<char>::const_iterator>& reader)
    {
        reader.ReadBuffer(name.data(), std::ssize(name));
        style = reader.Read<int32_t>();

        unk1 = ReadVector3(reader);
        unk2 = ReadVector3(reader);
    }
}
