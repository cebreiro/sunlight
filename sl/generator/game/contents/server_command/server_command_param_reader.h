#pragma once
#include <boost/lexical_cast.hpp>

namespace sunlight
{
    class ServerCommandParamReader
    {
    public:
        ServerCommandParamReader() = delete;
        ServerCommandParamReader(const ServerCommandParamReader& other) = delete;
        ServerCommandParamReader(ServerCommandParamReader&& other) noexcept = delete;
        ServerCommandParamReader& operator=(const ServerCommandParamReader& other) = delete;
        ServerCommandParamReader& operator=(ServerCommandParamReader&& other) noexcept = delete;

    public:
        explicit ServerCommandParamReader(const std::span<std::string>& params);

        template <int64_t N, typename T> requires std::is_arithmetic_v<T>
        bool TryParse(T& result) const;

        template <typename T> requires std::is_arithmetic_v<T>
        bool TryParse(int64_t index, T& result) const;

        bool Get(int64_t index, std::string& result) const;

        auto GetParamSize() const -> int64_t;

    private:
        std::span<std::string> _params;
    };

    template <int64_t N, typename T> requires std::is_arithmetic_v<T>
    bool ServerCommandParamReader::TryParse(T& result) const
    {
        return TryParse<T>(N, result);
    }

    template <typename T> requires std::is_arithmetic_v<T>
    bool ServerCommandParamReader::TryParse(int64_t index, T& result) const
    {
        assert(index >= 0);

        if (index >= std::ssize(_params))
        {
            return false;
        }

        try
        {
            result = boost::lexical_cast<T>(_params[index]);

            return true;
        }
        catch (...)
        {
        }

        return false;
    }
}
