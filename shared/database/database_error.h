#pragma once
#include <optional>
#include <string>
#include <stdexcept>

namespace boost::mysql
{
    class error_with_diagnostics;
}

namespace sunlight
{
    class DatabaseError
    {
    public:
        DatabaseError() = default;
        explicit DatabaseError(const std::exception& e);
        explicit DatabaseError(const boost::mysql::error_with_diagnostics& e);

        [[nodiscard]]
        operator bool() const;

        auto What() const -> std::string_view;

    private:
        std::optional<std::string> _message;
    };
}
