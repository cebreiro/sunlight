#pragma once
#include <expected>
#include <boost/asio.hpp>
#include "shared/execution/channel/channel.h"
#include "shared/execution/future/future.hpp"
#include "shared/network/io_error.h"
#include "shared/network/buffer/buffer.h"

namespace sunlight::execution
{
    class AsioStrand;
}

namespace sunlight
{
    class Buffer;

    class Socket : public std::enable_shared_from_this<Socket>
    {
        using completion_token_type = SharedPtrNotNull<future::SharedContext<std::expected<int64_t, IOError>>>;

    public:
        Socket() = delete;

        explicit Socket(SharedPtrNotNull<execution::AsioStrand> strand);

        bool IsOpen() const;
        bool IsSendPending() const;
        bool IsReceivePending() const;

        auto ConnectAsync(std::string address, uint16_t port,
            std::chrono::milliseconds retryInterval = std::chrono::milliseconds(5000),
            std::optional<std::stop_token> token = std::nullopt)
            -> Future<void>;
        void CloseAsync();

        auto SendAsync(Buffer buffer) -> Future<std::expected<int64_t, IOError>>;
        auto ReceiveAsync(Buffer& buffer) -> Future<std::expected<int64_t, IOError>>;

    private:
        void Send(Buffer buffer, completion_token_type promise);
        void OnSend();

        auto AllocCompletionToken() -> completion_token_type;
        void FreeCompletionToken(completion_token_type token);

    private:
        SharedPtrNotNull<execution::AsioStrand> _strand;
        boost::asio::ip::tcp::socket _socket;

        bool _shutdown = false;
        bool _sendPending = false;
        bool _receivePending = false;
        std::queue<std::pair<Buffer, completion_token_type>> _sendWaits;
        std::vector<completion_token_type> _recycleCompletionTokens;
    };
}
