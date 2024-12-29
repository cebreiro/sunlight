#pragma once
#include <boost/mysql.hpp>
#include <oneapi/tbb/concurrent_hash_map.h>
#include <oneapi/tbb/concurrent_queue.h>
#include "shared/type/borrowed.h"
#include "shared/execution/future/future.h"
#include "shared/database/connection/connection_pool_option.h"
#include "shared/log/log_service_interface.h"
#include "shared/service/service_locator.h"

namespace sunlight
{
    class ServiceLocator;
}

namespace sunlight::execution
{
    class AsioExecutor;
}

namespace sunlight::db
{
    class ConnectionPool : public std::enable_shared_from_this<ConnectionPool>
    {
    public:
        using Borrowed = Borrowed<boost::mysql::tcp_ssl_connection, ConnectionPool>;

    public:
        explicit ConnectionPool(SharedPtrNotNull<execution::AsioExecutor> executor);

        void Initialize(ServiceLocator& serviceLocator);

        void Start(const ConnectionPoolOption& option);
        void Stop();

        auto Borrow() -> Future<Borrowed>;
        void TakeBack(boost::mysql::tcp_ssl_connection& connection);

    private:
        struct Context;

        void Connect();
        auto TryPopConnection() -> std::unique_ptr<boost::mysql::tcp_ssl_connection>;
        void SetConnectionBorrowed(std::unique_ptr<boost::mysql::tcp_ssl_connection> connection);

        auto Ping() -> Future<void>;

    private:
        SharedPtrNotNull<execution::AsioExecutor> _executor;
        ServiceLocatorT<ILogService> _serviceLocator;
        ConnectionPoolOption _option;

        tbb::concurrent_queue<std::shared_ptr<Context>> _contexts;

        tbb::concurrent_queue<std::unique_ptr<boost::mysql::tcp_ssl_connection>> _idleConnections;
        tbb::concurrent_hash_map<size_t, std::unique_ptr<boost::mysql::tcp_ssl_connection>> _borrowedConnections;
        std::stop_source _pingStopSource;
    };
}
