#pragma once
#include <cstdint>
#include <span>
#include <optional>
#include <span>
#include <thread>

namespace sunlight::execution
{
    class GameExecutor;
}

namespace sunlight::buffer
{
    class Fragment
    {
    public:
        Fragment(const Fragment& other) = delete;;
        Fragment& operator=(const Fragment& other) = delete;

    public:
        Fragment() = default;
        Fragment(Fragment&& other) noexcept;
        Fragment& operator=(Fragment&& other) noexcept;

        ~Fragment();

    public:
        bool IsValid() const;

        auto SliceFront(int64_t size) -> std::optional<Fragment>;

        auto ShallowCopy() const -> Fragment;

        auto GetData() -> char*;
        auto GetData() const -> const char*;
        auto GetSize() const -> int64_t;

        auto GetSpan() const -> std::span<const char>;

        auto GetRefCount() const -> int64_t;

        static auto CreateFrom(std::span<const char> span) -> Fragment;
        static auto Create(int64_t size) -> Fragment;

    private:
        struct Header
        {
            std::atomic<int64_t> refCount = 0;
            int64_t size = 0;
        };

#pragma warning(disable:4324)
        struct alignas(std::hardware_destructive_interference_size) Impl
        {
            Header header;
        };
#pragma warning(default:4324)

    private:
        Impl* _impl = nullptr;
        int64_t _startOffset = 0;
        int64_t _size = 0;
    };
}

