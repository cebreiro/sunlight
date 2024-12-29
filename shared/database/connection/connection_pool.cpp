#include "connection_pool.h"

#include "shared/execution/executor/strand.h"
#include "shared/execution/executor/executor_coroutine_traits.h"
#include "shared/execution/executor/impl/asio_executor.h"
#include "shared/execution/executor/operation/schedule.h"
#include "shared/execution/future/future_coroutine_traits.h"
#include "shared/execution/future/operation/delay.h"
#include "shared/service/service_locator_log.h"

namespace sunlight::db
{
    ConnectionPool::ConnectionPool(SharedPtrNotNull<execution::AsioExecutor> executor)
        : _executor(std::move(executor))
    {
    }

    void ConnectionPool::Initialize(ServiceLocator& serviceLocator)
    {
        _serviceLocator = serviceLocator;
    }

    void ConnectionPool::Start(const ConnectionPoolOption& option)
    {
        _option = option;

        for (int32_t i = 0; i < _option.GetConnectionCount(); ++i)
        {
            Connect();
        }

        Ping();

        constexpr auto delay = std::chrono::seconds(60);
        _pingStopSource = Schedule(*_executor, [self = shared_from_this()]()
            {
                self->Ping();
            }, delay, delay);
    }

    void ConnectionPool::Stop()
    {
        _pingStopSource.request_stop();
    }

    auto ConnectionPool::Borrow() -> Future<Borrowed>
    {
        [[maybe_unused]]
        auto self = shared_from_this();

        while (true)
        {
            if (std::unique_ptr<boost::mysql::tcp_ssl_connection> connection = TryPopConnection(); connection)
            {
                boost::mysql::tcp_ssl_connection& reference = *connection;
                SetConnectionBorrowed(std::move(connection));

                co_return Borrowed(reference, shared_from_this());
            }
            else
            {
                co_await *_executor;
                assert(ExecutionContext::IsEqualTo(*_executor));

                co_await Delay(std::chrono::milliseconds(1));
                assert(ExecutionContext::IsEqualTo(*_executor));
            }
        }
    }

    void ConnectionPool::TakeBack(boost::mysql::tcp_ssl_connection& connection)
    {
        const size_t key = reinterpret_cast<size_t>(&connection);

        decltype(_borrowedConnections)::accessor accessor;
        if (_borrowedConnections.find(accessor, key))
        {
            _idleConnections.push(std::move(accessor->second));
            _borrowedConnections.erase(accessor);
        }
        else
        {
            assert(false);
        }
    }

    void ConnectionPool::Connect()
    {
        boost::asio::ssl::context sslContext(boost::asio::ssl::context::tls_client);
        auto connection = std::make_unique<boost::mysql::tcp_ssl_connection>(_executor->GetIoContext(), sslContext);

        connection->connect(_option.GetEndPoint(), _option.GetHandShakeParams());

        _idleConnections.push(std::move(connection));
    }

    auto ConnectionPool::TryPopConnection() -> std::unique_ptr<boost::mysql::tcp_ssl_connection>
    {
        std::unique_ptr<boost::mysql::tcp_ssl_connection> connection;
        (void)_idleConnections.try_pop(connection);

        return connection;
    }

    void ConnectionPool::SetConnectionBorrowed(std::unique_ptr<boost::mysql::tcp_ssl_connection> connection)
    {
        const size_t key = reinterpret_cast<size_t>(connection.get());

        decltype(_borrowedConnections)::accessor accessor;
        if (_borrowedConnections.insert(accessor, key))
        {
            accessor->second = std::move(connection);
        }
        else
        {
            assert(false);
        }
    }

    auto ConnectionPool::Ping() -> Future<void>
    {
        [[maybe_unused]]
        auto self = shared_from_this();

        auto holder = this->shared_from_this();

        co_await *_executor;
        assert(ExecutionContext::IsEqualTo(*_executor));

        std::vector<std::unique_ptr<boost::mysql::tcp_ssl_connection>> connections;
        std::unique_ptr<boost::mysql::tcp_ssl_connection> context;

        while (_idleConnections.try_pop(context))
        {
            connections.push_back(std::move(context));
        }

        for (std::unique_ptr<boost::mysql::tcp_ssl_connection>& connection : connections)
        {
            boost::mysql::error_code ec;
            boost::mysql::diagnostics diagnostics;
            connection->ping(ec, diagnostics);

            if (ec)
            {
                SUNLIGHT_LOG_CRITICAL(_serviceLocator,
                    fmt::format("[connection_pool] database ping error. error: {}, diagnostics: {}",
                        ec.message(), diagnostics.server_message().data()));
            }
            else
            {
                _idleConnections.push(std::move(connection));
            }
        }
    }
}
