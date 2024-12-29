#include "database_error.h"

#include <boost/mysql/error_with_diagnostics.hpp>
#include <sstream>

namespace sunlight
{
    DatabaseError::DatabaseError(const std::exception& e)
        : _message(e.what())
    {
    }

    DatabaseError::DatabaseError(const boost::mysql::error_with_diagnostics& e)
        : _message([&e]()
        {
                std::ostringstream oss;

            const boost::mysql::diagnostics& diagnostics = e.get_diagnostics();
            const boost::mysql::string_view serverMessage = diagnostics.server_message();
            const boost::mysql::string_view clientMessage = diagnostics.client_message();

            if (!serverMessage.empty())
            {
                oss << "server_diagnostics: " << serverMessage;
            }

            if (!clientMessage.empty())
            {
                if (!serverMessage.empty())
                {
                    oss << ", ";
                }

                oss << "client_diagnostics: " << clientMessage;
            }

            return oss.str();
        }())
    {
    }

    DatabaseError::operator bool() const
    {
        return _message.has_value();
    }

    auto DatabaseError::What() const -> std::string_view
    {
        if (_message)
        {
            return *_message;
        }

        return "";
    }
}
