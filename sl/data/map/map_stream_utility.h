#pragma once

namespace sunlight
{
    auto ReadVector3(StreamReader<std::vector<char>::const_iterator>& reader) -> Eigen::Vector3f;
    auto ReadMatrix4x4(StreamReader<std::vector<char>::const_iterator>& reader) -> Eigen::Matrix4f;
}
