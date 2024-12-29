#pragma once
#include <cstdint>
#include <atomic>
#include <cassert>
#include <coroutine>
#include <concepts>
#include <variant>
#include <exception>
#include <functional>
#include <mutex>
#include <queue>
#include <boost/container/small_vector.hpp>
#include "shared/execution/channel/channel_status.h"
#include "shared/execution/channel/channel_signal.h"

namespace sunlight::channel
{
    template <std::move_constructible T>
    class MpmcChannel
    {
    public:
        using value_type = T;
        using item_type = std::variant<value_type, std::exception_ptr>;

    public:
        MpmcChannel(const MpmcChannel& other) = delete;
        MpmcChannel(MpmcChannel&& other) noexcept = delete;
        MpmcChannel& operator=(const MpmcChannel& other) = delete;
        MpmcChannel& operator=(MpmcChannel&& other) noexcept = delete;

        MpmcChannel() = default;
        ~MpmcChannel() = default;

        bool Empty() const;
        bool IsOpen() const;
        bool IsClosed() const;

        bool Close();

        bool Receive(value_type& result);

        template <std::ranges::range R> requires std::convertible_to<std::ranges::range_value_t<R>, T>
        void Send(R&& range, ChannelSignal signal = ChannelSignal::None);
        void Send(value_type item, ChannelSignal signal = ChannelSignal::None);
        void Send(const std::exception_ptr& exception, ChannelSignal signal = ChannelSignal::None);

        bool TryPop(item_type& item);

        void AddOrExecuteSignalHandler(std::move_only_function<void()> handler);
        void NotifyOne();
        void NotifyAll();

    private:
        void NotifyOne(std::unique_lock<std::mutex>& lock);
        void NotifyAll(std::unique_lock<std::mutex>& lock);

    private:
        std::atomic<ChannelStatus> _status = ChannelStatus::Open;
        mutable std::mutex _mutex;
        std::queue<item_type> _items;
        boost::container::small_vector<std::move_only_function<void()>, 8> _signalHandlers;
    };

    template <std::move_constructible T>
    bool MpmcChannel<T>::Empty() const
    {
        std::lock_guard lock(_mutex);
        return _items.empty();
    }

    template <std::move_constructible T>
    bool MpmcChannel<T>::IsOpen() const
    {
        return _status.load() == ChannelStatus::Open;
    }

    template <std::move_constructible T>
    bool MpmcChannel<T>::IsClosed() const
    {
        return _status.load() == ChannelStatus::Closed;
    }

    template <std::move_constructible T>
    bool MpmcChannel<T>::Close()
    {
        ChannelStatus expected = ChannelStatus::Open;
        if (!_status.compare_exchange_strong(expected, ChannelStatus::Closed))
        {
            return false;
        }

        std::unique_lock lock(_mutex);
        NotifyAll(lock);

        return true;
    }

    template <std::move_constructible T>
    bool MpmcChannel<T>::Receive(value_type& result)
    {
        std::lock_guard lock(_mutex);

        if (_items.empty())
        {
            return false;
        }

        item_type item = std::move(_items.front());
        _items.pop();

        if (std::holds_alternative<std::exception_ptr>(item))
        {
            std::rethrow_exception(std::get<std::exception_ptr>(item));
        }

        result = std::move(std::get<value_type>(item));

        return true;
    }

    template <std::move_constructible T>
    void MpmcChannel<T>::AddOrExecuteSignalHandler(std::move_only_function<void()> handler)
    {
        assert(handler);

        {
            std::lock_guard lock(_mutex);

            if (IsOpen() && _items.empty())
            {
                _signalHandlers.push_back(std::move(handler));
                return;
            }
        }

        if (handler)
        {
            handler();
        }
    }

    template <std::move_constructible T>
    void MpmcChannel<T>::NotifyOne()
    {
        std::unique_lock lock(_mutex);

        NotifyOne(lock);
    }

    template <std::move_constructible T>
    void MpmcChannel<T>::NotifyAll()
    {
        std::unique_lock lock(_mutex);

        NotifyAll(lock);
    }

    template <std::move_constructible T>
    void MpmcChannel<T>::NotifyOne(std::unique_lock<std::mutex>& lock)
    {
        assert(lock.owns_lock());

        if (_signalHandlers.empty())
        {
            return;
        }

        std::move_only_function<void()> handler = std::move(_signalHandlers.front());
        _signalHandlers.erase(_signalHandlers.begin());

        {
            lock.unlock();

            handler();

            lock.lock();
        }

        assert(lock.owns_lock());
    }

    template <std::move_constructible T>
    void MpmcChannel<T>::NotifyAll(std::unique_lock<std::mutex>& lock)
    {
        assert(lock.owns_lock());

        decltype(_signalHandlers) handlers = std::exchange(_signalHandlers, {});
        {
            lock.unlock();

            for (std::move_only_function<void()>& handler : handlers)
            {
                handler();
            }

            lock.lock();
        }

        assert(lock.owns_lock());
    }

    template <std::move_constructible T>
    template <std::ranges::range R> requires std::convertible_to<std::ranges::range_value_t<R>, T>
    void MpmcChannel<T>::Send(R&& range, ChannelSignal signal)
    {
        std::unique_lock lock(_mutex);

        for (decltype(auto) value : range)
        {
            _items.push(std::move(value));
        }

        switch (signal)
        {
        case ChannelSignal::None:
            break;
        case ChannelSignal::NotifyOne:
            NotifyOne(lock);
            break;
        case ChannelSignal::NotifyAll:
            NotifyAll(lock);
            break;
        default:;
        }
    }

    template <std::move_constructible T>
    void MpmcChannel<T>::Send(value_type item, ChannelSignal signal)
    {
        std::unique_lock lock(_mutex);

        _items.push(std::move(item));

        switch (signal)
        {
        case ChannelSignal::None:
            break;
        case ChannelSignal::NotifyOne:
            NotifyOne(lock);
            break;
        case ChannelSignal::NotifyAll:
            NotifyAll(lock);
            break;
        default:;
        }
    }

    template <std::move_constructible T>
    void MpmcChannel<T>::Send(const std::exception_ptr& exception, ChannelSignal signal)
    {
        std::unique_lock lock(_mutex);

        _items.push(exception);

        switch (signal)
        {
        case ChannelSignal::None:
            break;
        case ChannelSignal::NotifyOne:
            NotifyOne(lock);
            break;
        case ChannelSignal::NotifyAll:
            NotifyAll(lock);
            break;
        default:;
        }
    }

    template <std::move_constructible T>
    bool MpmcChannel<T>::TryPop(item_type& item)
    {
        std::lock_guard lock(_mutex);

        if (_items.empty())
        {
            return false;
        }

        item = std::move(_items.front());
        _items.pop();

        return true;
    }
}
