#pragma once

namespace sunlight::sox
{
    class ISoxTable
    {
    public:
        virtual ~ISoxTable() = default;

        virtual void LoadFromFile(const std::filesystem::path& path) = 0;
    };
}
