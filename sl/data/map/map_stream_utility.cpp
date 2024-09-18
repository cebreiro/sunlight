#include "map_stream_utility.h"

namespace sunlight
{
    auto ReadVector3(StreamReader<std::vector<char>::const_iterator>& reader) -> Eigen::Vector3f
    {
        Eigen::Vector3f result;
        result.x() = reader.Read<float>();
        result.y() = reader.Read<float>();
        result.z() = reader.Read<float>();

        return result;
    }

    auto ReadMatrix4x4(StreamReader<std::vector<char>::const_iterator>& reader) -> Eigen::Matrix4f
    {
        Eigen::Matrix4f result;
        
        for (int32_t i = 0; i < 4; ++i)
        {
            for (int32_t j = 0; j < 4; ++j)
            {
                result(i, j) = reader.Read<float>();
            }
        }

        return result;
    }
}
