#include "io_error.h"

namespace sunlight
{
    IOError::IOError(const boost::system::error_code& ec)
        : value(ec.value())
        , message(ec.message())
    {
    }
}
