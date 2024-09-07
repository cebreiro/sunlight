#pragma once
#include <cstdint>
#include <atomic>

namespace sunlight
{
    template <typename T>
    class RuntimeTypeId
    {
    public:
        template <typename U>
        static auto Get() -> int64_t;

    private:
        static std::atomic<int64_t> _nextId;
    };

    template <typename T>
    template <typename U>
    auto RuntimeTypeId<T>::Get() -> int64_t
    {
        static int64_t id{ _nextId.fetch_add(1) };
        return id;
    }

    template <typename T>
    std::atomic<int64_t> RuntimeTypeId<T>::_nextId = 0;
}
