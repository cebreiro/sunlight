#pragma once
#include <cstdint>
#include <optional>
#include <boost/asio/ip/tcp.hpp>
#include <boost/mysql/handshake_params.hpp>

namespace sunlight::db
{
    class ConnectionPoolOption
    {
    public:
        ConnectionPoolOption() = default;
        ConnectionPoolOption(
            boost::asio::ip::tcp::endpoint endPoint, const boost::mysql::handshake_params& handShakeParams,
            int32_t connectionCount);

        auto GetEndPoint() const -> const boost::asio::ip::tcp::endpoint&;
        auto GetHandShakeParams() const -> const boost::mysql::handshake_params&;
        auto GetConnectionCount() const -> int32_t;

    private:
        boost::asio::ip::tcp::endpoint _endPoint;
        std::optional<boost::mysql::handshake_params> _handShakeParams = std::nullopt;
        int32_t _connectionCount = 0;
    };
}
