#include "snowflake.h"

namespace sunlight::snowflake
{
    auto Clock::Now() -> uint64_t
    {
        using std::chrono::milliseconds;
        using std::chrono::system_clock;
        using std::chrono::duration_cast;

        return duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
    }
}
