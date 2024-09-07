#include "socket.h"

#include <boost/scope/scope_exit.hpp>

#include "shared/app/app_log.h"
#include "shared/network/io_error.h"
#include "shared/execution/executor/executor_coroutine_traits.h"
#include "shared/execution/executor/impl/asio_strand.h"
#include "shared/execution/executor/operation/dispatch.h"
#include "shared/execution/executor/operation/schedule.h"

namespace sunlight
{
    Socket::Socket(SharedPtrNotNull<execution::AsioStrand> strand)
        : _strand(std::move(strand))
        , _socket(_strand->GetImpl())
    {
    }

    bool Socket::IsOpen() const
    {
        return _socket.is_open();
    }

    bool Socket::IsSendPending() const
    {
        return _sendPending;
    }

    bool Socket::IsReceivePending() const
    {
        return _receivePending;
    }

    auto Socket::ConnectAsync(std::string address, uint16_t port,
        std::chrono::milliseconds retryInterval, std::optional<std::stop_token> token)
        -> Future<void>
    {
        [[maybe_unused]]
        const auto self = shared_from_this();

        const boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::make_address(address), port);

        if (!ExecutionContext::IsEqualTo(*_strand))
        {
            co_await *_strand;
        }

        assert(ExecutionContext::IsEqualTo(*_strand));

        auto context = std::make_shared<future::SharedContext<boost::system::error_code>>();

        while (true)
        {
            context->Reset();

            _socket.async_connect(endpoint,
                [context](const boost::system::error_code& ec) mutable
                {
                    context->OnSuccess(ec);
                });

            const boost::system::error_code& errorCode = co_await Future(context);
            if (!errorCode)
            {
                assert(ExecutionContext::IsEqualTo(*_strand));
                assert(_socket.is_open());

                self->_shutdown = false;

                co_return;
            }

            if (token && token->stop_requested())
            {
                co_return;
            }

            co_await Delay(retryInterval);
            assert(ExecutionContext::IsEqualTo(*_strand));
        }
    }

    void Socket::CloseAsync()
    {
        Post(*_strand, [self = shared_from_this()]()
            {
                self->_shutdown = true;

                boost::system::error_code ec;
                self->_socket.close(ec);
            });
    }

    auto Socket::SendAsync(Buffer buffer) -> Future<std::expected<int64_t, IOError>>
    {
        [[maybe_unused]]
        auto self = shared_from_this();

        if (!ExecutionContext::IsEqualTo(*_strand))
        {
            co_await *_strand;
        }

        if (_shutdown)
        {
            co_return std::unexpected(IOError(boost::system::error_code(boost::asio::error::operation_aborted)));
        }

        assert(ExecutionContext::IsEqualTo(*_strand));

        SharedPtrNotNull<future::SharedContext<std::expected<int64_t, IOError>>> completionToken = AllocCompletionToken();
        boost::scope::scope_exit free([this, completionToken]() mutable
            {
                FreeCompletionToken(std::move(completionToken));
            });

        if (_sendPending)
        {
            _sendWaits.emplace(std::move(buffer), completionToken);

            co_return co_await Future(completionToken);
        }

        _sendPending = true;

        Send(std::move(buffer), completionToken);

        co_return co_await Future(completionToken);
    }

    auto Socket::ReceiveAsync(Buffer& buffer) -> Future<std::expected<int64_t, IOError>>
    {
        [[maybe_unused]]
        auto self = shared_from_this();

        if (!ExecutionContext::IsEqualTo(*_strand))
        {
            co_await *_strand;
        }

        assert(ExecutionContext::IsEqualTo(*_strand));

        _receivePending = true;

        completion_token_type completionToken = AllocCompletionToken();
        boost::scope::scope_exit free([this, completionToken]() mutable
            {
                FreeCompletionToken(std::move(completionToken));

                _receivePending = false;
            });

        boost::container::small_vector<boost::asio::mutable_buffer, 4> mutableBuffers;
        for (buffer::Fragment& fragment : buffer.GetFragmentContainer())
        {
            mutableBuffers.emplace_back(fragment.GetData(), fragment.GetSize());
        }

        _socket.async_receive(mutableBuffers,
            bind_executor(_strand->GetImpl(),
                [token = completionToken](const boost::system::error_code& ec, size_t bytes) mutable
                {
                    if (ec)
                    {
                        token->OnSuccess(std::unexpected<IOError>(ec));
                    }
                    else
                    {
                        token->OnSuccess(bytes);
                    }
                }));

        co_return co_await Future(completionToken);
    }

    void Socket::OnSend()
    {
        assert(ExecutionContext::IsEqualTo(*_strand));

        if (_sendWaits.empty())
        {
            _sendPending = false;

            return;
        }

        if (_shutdown)
        {
            while (!_sendWaits.empty())
            {
                completion_token_type& token = _sendWaits.front().second;

                token->OnSuccess(std::unexpected(IOError(boost::system::error_code(boost::asio::error::operation_aborted))));

                _sendWaits.pop();
            }

            return;
        }

        auto [buffer, promise] = std::move(_sendWaits.front());
        _sendWaits.pop();

        Send(std::move(buffer), std::move(promise));
    }

    void Socket::Send(Buffer buffer, completion_token_type token)
    {
        boost::container::small_vector<boost::asio::const_buffer, 4> constBuffers;

        for (const buffer::Fragment& fragment : buffer.GetFragmentContainer())
        {
            constBuffers.emplace_back(fragment.GetData(), fragment.GetSize());
        }

        async_write(_socket, constBuffers,
            bind_executor(_strand->GetImpl(),
                [t = std::move(token), hold = std::move(buffer), self = shared_from_this()](const boost::system::error_code& ec, size_t bytes) mutable
                {
                    if (ec)
                    {
                        t->OnSuccess(std::unexpected<IOError>(ec));
                    }
                    else
                    {
                        t->OnSuccess(static_cast<int64_t>(bytes));
                    }

                    Dispatch(*self->_strand, [self]()
                        {
                            self->OnSend();
                        });
                }));
    }

    auto Socket::AllocCompletionToken() -> completion_token_type
    {
        completion_token_type result;

        if (_recycleCompletionTokens.empty())
        {
            result = std::make_shared<future::SharedContext<std::expected<int64_t, IOError>>>();
        }
        else
        {
            result = std::move(_recycleCompletionTokens.back());
            _recycleCompletionTokens.pop_back();
        }

        return result;
    }

    void Socket::FreeCompletionToken(completion_token_type token)
    {
        token->Reset();

        _recycleCompletionTokens.emplace_back(std::move(token));
    }
}
