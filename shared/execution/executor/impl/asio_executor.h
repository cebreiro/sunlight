#pragma once
#include <cstdint>
#include <vector>
#include <thread>
#include <future>
#include <functional>
#include <boost/asio.hpp>
#include "shared/execution/executor/executor_interface.h"

namespace sunlight::execution
{
    class AsioStrand;

    class AsioExecutor final : public IExecutor, public std::enable_shared_from_this<AsioExecutor>
    {
    public:
        explicit AsioExecutor(int64_t workerCount);
        ~AsioExecutor() override;

        void Run();
        void Stop() override;
        void Join(std::vector<boost::system::error_code>* errors);

        void Post(const std::function<void()>& function) override;
        void Post(std::move_only_function<void()> function) override;

        void Dispatch(const std::function<void()>& function) override;
        void Dispatch(std::move_only_function<void()> function) override;

        void Delay(const std::function<void()>& function, std::chrono::milliseconds milliseconds);
        void Delay(std::move_only_function<void()> function, std::chrono::milliseconds milliseconds);

        auto SharedFromThis() -> SharedPtrNotNull<IExecutor> override;
        auto SharedFromThis() const -> SharedPtrNotNull<const IExecutor> override;

        auto GetIoContext() -> boost::asio::io_context&;
        auto GetIoContext() const -> const boost::asio::io_context&;

        auto GetConcurrency() const -> int64_t override;

        auto MakeStrand() -> SharedPtrNotNull<AsioStrand>;

    private:
        int64_t _workerCount = 0;
        std::vector<std::jthread> _workers;
        std::vector<std::future<boost::system::error_code>> _workErrors;
        boost::asio::io_context _ioContext;
        boost::asio::executor_work_guard<decltype(_ioContext)::executor_type> _workGuard;
    };
}
