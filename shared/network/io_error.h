#pragma once
#include <boost/system/error_code.hpp>

namespace sunlight
{
    struct IOError
    {
        IOError() = default;

        explicit IOError(const boost::system::error_code& ec);

        int32_t value = 0;
        std::string message;
    };
}
