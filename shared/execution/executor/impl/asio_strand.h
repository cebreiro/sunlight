#pragma once
#include <chrono>
#include <future>
#include <functional>
#include <boost/asio.hpp>
#include "shared/execution/executor/executor_interface.h"

namespace sunlight::execution
{
    class AsioStrand final : public IExecutor, public std::enable_shared_from_this<AsioStrand>
    {
    public:
        explicit AsioStrand(boost::asio::strand<boost::asio::io_context::executor_type> strand);
        ~AsioStrand() override;

        void Stop() override;

        void Post(const std::function<void()>& function) override;
        void Post(std::move_only_function<void()> function) override;

        void Dispatch(const std::function<void()>& function) override;
        void Dispatch(std::move_only_function<void()> function) override;

        auto GetConcurrency() const -> int64_t override;

        auto SharedFromThis() -> SharedPtrNotNull<IExecutor> override;
        auto SharedFromThis() const->SharedPtrNotNull<const IExecutor> override;

        auto GetImpl() -> boost::asio::strand<boost::asio::io_context::executor_type>&;
        auto GetImpl() const -> const boost::asio::strand<boost::asio::io_context::executor_type>&;

    private:
        boost::asio::strand<boost::asio::io_context::executor_type> _strand;
    };
}
