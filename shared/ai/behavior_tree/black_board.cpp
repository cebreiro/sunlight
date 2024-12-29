#include "black_board.h"

namespace sunlight::bt
{
    void BlackBoard::Clear()
    {
        _container.clear();
    }

    bool BlackBoard::Contains(const std::string& key) const
    {
        return _container.contains(key);
    }

    bool BlackBoard::Remove(const std::string& key)
    {
        return _container.erase(key);
    }
}
