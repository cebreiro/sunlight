#include "sox_table_set.h"

namespace sunlight
{
    SoxTableSet::SoxTableSet(const std::filesystem::path& path)
        : _path(path)
    {
    }

    SoxTableSet::~SoxTableSet()
    {
    }

    void SoxTableSet::Clear()
    {
        _tables.clear();
    }
}
