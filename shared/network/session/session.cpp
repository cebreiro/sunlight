#include "session.h"

#include <boost/container/small_vector.hpp>
#include "shared/execution/executor/impl/asio_strand.h"
#include "shared/log/log_service_interface.h"

namespace sunlight
{
    Session::Session(id_type id, SharedPtrNotNull<session::event_channel_type> channel,
        boost::asio::ip::tcp::socket socket, SharedPtrNotNull<execution::AsioStrand> strand,
        std::shared_ptr<ILogService> logService)
        : _id(id)
        , _channel(std::move(channel))
        , _socket(std::move(socket))
        , _strand(std::move(strand))
        , _logService(std::move(logService))
        , _localAddress(_socket.local_endpoint().address().to_string())
        , _localPort(_socket.local_endpoint().port())
        , _remoteAddress(_socket.remote_endpoint().address().to_string())
        , _remotePort(_socket.remote_endpoint().port())
    {
        assert(_socket.is_open());

        ExpandReceiveBuffer();
    }

    Session::~Session()
    {
        session::DestructEvent event{
            .id = _id,
        };

        _channel->Send(std::move(event), channel::ChannelSignal::NotifyOne);
        _channel->Close();
    }

    void Session::StartReceive()
    {
        post(_strand->GetImpl(), [self = shared_from_this()]()
            {
                self->ReceiveAsync();
            });
    }

    void Session::Send(Buffer buffer)
    {
        post(_strand->GetImpl(), [self = shared_from_this(), buffer = std::move(buffer)]() mutable
            {
                self->SendAsync(std::move(buffer));
            });
    }

    void Session::Close()
    {
        post(_strand->GetImpl(), [self = shared_from_this()]()
            {
                boost::system::error_code ec;
                self->_socket.close(ec);

                if (ec)
                {
                    if (self->_logService)
                    {
                        self->_logService->Log(LogLevel::Error,
                            fmt::format("[{}] error on socket close. error: {}",
                                *self, ec.message()),
                            std::source_location::current());
                    }
                }
            });
    }

    bool Session::IsOpen() const
    {
        return _socket.is_open();
    }

    void Session::SetNoDelay(bool value)
    {
        post(_strand->GetImpl(), [self = shared_from_this(), value]()
            {
                boost::asio::ip::tcp::no_delay option(value);
                boost::system::error_code ec;

                self->_socket.set_option(option, ec);

                if (ec)
                {
                    if (self->_logService)
                    {
                        self->_logService->Log(LogLevel::Error,
                            fmt::format("[{}] error on set tcp no delay. value: {}, error: {}",
                                *self, value, ec.message()),
                            std::source_location::current());
                    }
                }
            });
    }

    auto Session::GetId() const -> id_type
    {
        return _id;
    }

    auto Session::GetLocalAddress() const -> const std::string&
    {
        return _localAddress;
    }

    auto Session::GetLocalPort() const -> uint16_t
    {
        return _localPort;
    }

    auto Session::GetRemoteAddress() const -> const std::string&
    {
        return _remoteAddress;
    }

    auto Session::GetRemotePort() const -> uint16_t
    {
        return _remotePort;
    }

    auto Session::GetStrand() -> execution::AsioStrand&
    {
        return *_strand;
    }

    auto Session::GetStrand() const -> const execution::AsioStrand&
    {
        return *_strand;
    }

    void Session::ReceiveAsync()
    {
        assert(_mutableBuffers.empty());
        assert(_receiveBuffer.GetSize() >= RECEIVE_BUFFER_MIN_SIZE);

        for (buffer::Fragment& fragment : _receiveBuffer.GetFragmentContainer())
        {
            assert(fragment.IsValid());

            _mutableBuffers.emplace_back(fragment.GetData(), fragment.GetSize());
        }

        _socket.async_read_some(_mutableBuffers, bind_executor(_strand->GetImpl(),
            [self = shared_from_this()](const boost::system::error_code& ec, size_t bytes)
            {
                if (ec)
                {
                    self->HandleError(ec);
                    return;
                }

                self->HandleReceive(static_cast<int64_t>(bytes));
            }));
    }

    void Session::HandleReceive(int64_t bytes)
    {
        _mutableBuffers.clear();

        Buffer buffer;
        if (!_receiveBuffer.SliceFront(buffer, bytes))
        {
            if (_logService)
            {
                _logService->Log(LogLevel::Critical,
                    fmt::format("[session_{}] session receive completion. receive buffer error. receive_buffer: {}, receive: {}",
                        GetId(), _receiveBuffer.GetSize(), bytes),
                    std::source_location::current());
            }

            assert(false);
            
            Close();

            return;
        }

        SendReceiveEvent(std::move(buffer));

        ExpandReceiveBuffer();
        ReceiveAsync();
    }

    void Session::HandleError(const boost::system::error_code& ec)
    {
        session::IoErrorEvent event{
            .id = _id,
            .errorCode = ec,
        };

        _channel->Send(event, channel::ChannelSignal::NotifyOne);
    }

    void Session::SendReceiveEvent(Buffer buffer)
    {
        session::ReceiveEvent event{
            .id = _id,
            .buffer = std::move(buffer),
        };

        _channel->Send(std::move(event), channel::ChannelSignal::NotifyOne);
    }

    void Session::ExpandReceiveBuffer()
    {
        if (_receiveBuffer.GetSize() >= RECEIVE_BUFFER_MIN_SIZE)
        {
            return;
        }

        _receiveBuffer.Add(buffer::Fragment::Create(RECEIVE_BUFFER_EXPAND_SIZE));
    }

    void Session::SendAsync(Buffer buffer)
    {
        assert(buffer.GetSize() > 0);

        if (_sendPending)
        {
            _sendWaitQueue.emplace_back(std::move(buffer));

            return;
        }

        assert(_sendWaitQueue.empty());
        assert(_sendBuffer.Empty());

        _sendPending = true;
        _sendBuffer.MergeBack(std::move(buffer));

        constexpr int64_t mtuSize = 1460;
        constexpr int64_t maxSize = mtuSize;

        if (_sendBuffer.GetSize() > maxSize)
        {
            Buffer temp;

            [[maybe_unused]]
            const bool sliced = _sendBuffer.SliceFront(temp, maxSize);
            assert(sliced);

            std::swap(temp, _sendBuffer);

            _sendWaitQueue.insert(_sendWaitQueue.begin(), std::move(temp));
        }

        WriteAsync(ConfigureConstBuffer(_sendBuffer));
    }

    void Session::OnWriteAsync(int64_t bytes)
    {
        assert(!_sendBuffer.Empty());
        assert(!_constBuffers.empty());

        _constBuffers.clear();
        if (!_sendPending)
        {
            if (_logService)
            {
                _logService->Log(LogLevel::Critical,
                    fmt::format("[session_{}] session send completion. send pending state error", GetId()),
                    std::source_location::current());
            }

            assert(false);

            Close();

            return;
        }

        Buffer buffer;
        if (!_sendBuffer.SliceFront(buffer, bytes))
        {
            if (_logService)
            {
                _logService->Log(LogLevel::Critical,
                    fmt::format("[session_{}] session send completion. send buffer error", GetId()),
                    std::source_location::current());
            }

            assert(false);

            Close();

            return;
        }

        if (_sendBuffer.GetSize() > 0)
        {
            if (_logService)
            {
                _logService->Log(LogLevel::Critical,
                    fmt::format("[session_{}] session send completion. partial transmit occurred. request: {}, send: {}",
                        GetId(), _sendBuffer.GetSize() + bytes, bytes),
                    std::source_location::current());
            }

            assert(false);

            WriteAsync(ConfigureConstBuffer(_sendBuffer));

            return;
        }

        _sendPending = false;
        _sendBuffer.Clear();

        if (!_sendWaitQueue.empty())
        {
            SendAsync(FlushSendWaitQueue());
        }
    }

    auto Session::FlushSendWaitQueue() -> Buffer
    {
        assert(!_sendWaitQueue.empty());

        Buffer sendBuffer;

        for (Buffer& buffer : _sendWaitQueue)
        {
            sendBuffer.MergeBack(std::move(buffer));
        }

        _sendWaitQueue.clear();

        return sendBuffer;
    }

    auto Session::ConfigureConstBuffer(Buffer& buffer) -> const std::vector<boost::asio::const_buffer>&
    {
        assert(buffer.GetSize() > 0);
        assert(_constBuffers.empty());

        for (const buffer::Fragment& fragment : buffer.GetFragmentContainer())
        {
            assert(fragment.IsValid());

            _constBuffers.emplace_back(fragment.GetData(), fragment.GetSize());
        }

        return _constBuffers;
    }

    void Session::WriteAsync(const std::vector<boost::asio::const_buffer>& constBuffer)
    {
        async_write(_socket, constBuffer, bind_executor(_strand->GetImpl(),
            [self = shared_from_this()](const boost::system::error_code& ec, size_t bytes)
            {
                if (ec)
                {
                    self->HandleError(ec);

                    return;
                }

                self->OnWriteAsync(static_cast<int64_t>(bytes));
            }));
    }

}
