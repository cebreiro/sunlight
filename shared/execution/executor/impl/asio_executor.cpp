#include "asio_executor.h"

#include <cassert>
#include <future>

#include "shared/execution/executor/impl/asio_strand.h"

namespace sunlight::execution
{
    AsioExecutor::AsioExecutor(int64_t workerCount)
        : _workerCount(workerCount)
        , _workGuard(make_work_guard(_ioContext))
    {
    }

    AsioExecutor::~AsioExecutor()
    {
        if (!_ioContext.stopped())
        {
            Stop();
            Join(nullptr);
        }
    }

    void AsioExecutor::Post(const std::function<void()>& function)
    {
        post(_ioContext, function);
    }

    void AsioExecutor::Post(std::move_only_function<void()> function)
    {
        post(_ioContext, std::move(function));
    }

    void AsioExecutor::Dispatch(const std::function<void()>& function)
    {
        dispatch(_ioContext, function);
    }

    void AsioExecutor::Dispatch(std::move_only_function<void()> function)
    {
        dispatch(_ioContext, std::move(function));
    }

    void AsioExecutor::Delay(const std::function<void()>& function, std::chrono::milliseconds milliseconds)
    {
        auto timer = std::make_shared<boost::asio::steady_timer>(_ioContext);
        timer->expires_after(milliseconds);
        timer->async_wait([timer, function](const boost::system::error_code& ec) mutable
            {
                if (ec)
                {
                    return;
                }

                function();
            });
    }

    void AsioExecutor::Delay(std::move_only_function<void()> function, std::chrono::milliseconds milliseconds)
    {
        auto timer = std::make_shared<boost::asio::steady_timer>(_ioContext);
        timer->expires_after(milliseconds);
        timer->async_wait([timer, function = std::move(function)](const boost::system::error_code& ec) mutable
            {
                if (ec)
                {
                    return;
                }

                function();
            });
    }

    auto AsioExecutor::SharedFromThis() -> SharedPtrNotNull<IExecutor>
    {
        return shared_from_this();
    }

    auto AsioExecutor::SharedFromThis() const -> SharedPtrNotNull<const IExecutor>
    {
        return shared_from_this();
    }

    auto AsioExecutor::GetIoContext() -> boost::asio::io_context&
    {
        return _ioContext;
    }

    auto AsioExecutor::GetIoContext() const -> const boost::asio::io_context&
    {
        return _ioContext;
    }

    auto AsioExecutor::GetConcurrency() const -> int64_t
    {
        return _workerCount;
    }

    auto AsioExecutor::MakeStrand() -> SharedPtrNotNull<AsioStrand>
    {
        return std::make_shared<AsioStrand>(make_strand(GetIoContext()));
    }

    void AsioExecutor::Run()
    {
        assert(_workers.empty());

        _workErrors.reserve(_workerCount);

        for (int64_t i = 0; i < _workerCount; ++i)
        {
            std::promise<boost::system::error_code> promise;
            _workErrors.emplace_back(promise.get_future());

            _workers.emplace_back([this, promise = std::move(promise)]() mutable
                {
                    boost::system::error_code ec;
                    _ioContext.run(ec);

                    promise.set_value(ec);
                });
        }
    }

    void AsioExecutor::Stop()
    {
        _ioContext.stop();
    }

    void AsioExecutor::Join(std::vector<boost::system::error_code>* errors)
    {
        assert(_ioContext.stopped());

        for (std::future<boost::system::error_code>& future : _workErrors)
        {
            const boost::system::error_code& ec = future.get();
            if (ec && errors != nullptr)
            {
                errors->push_back(ec);
            }
        }

        _workErrors.clear();
        _workers.clear();
    }
}
