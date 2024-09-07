#include "connection_pool_option.h"

namespace sunlight::db
{
    ConnectionPoolOption::ConnectionPoolOption(
        boost::asio::ip::tcp::endpoint endPoint, const boost::mysql::handshake_params& handShakeParams,
        int32_t connectionCount)
        : _endPoint(std::move(endPoint))
        , _handShakeParams(handShakeParams)
        , _connectionCount(connectionCount)
    {
        _connectionCount = std::max(0, _connectionCount);
    }

    auto ConnectionPoolOption::GetEndPoint() const -> const boost::asio::ip::tcp::endpoint&
    {
        return _endPoint;
    }

    auto ConnectionPoolOption::GetHandShakeParams() const -> const boost::mysql::handshake_params&
    {
        assert(_handShakeParams.has_value());
        return *_handShakeParams;
    }

    auto ConnectionPoolOption::GetConnectionCount() const -> int32_t
    {
        return _connectionCount;
    }
}
