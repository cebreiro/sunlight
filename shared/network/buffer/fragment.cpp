#include "fragment.h"

#include <cassert>
#include <tbb/scalable_allocator.h>

namespace sunlight::buffer
{
    Fragment::Fragment(Fragment&& other) noexcept
        : _impl(std::exchange(other._impl, nullptr))
        , _startOffset(std::exchange(other._startOffset, 0))
        , _size(std::exchange(other._size, 0))
    {
    }

    Fragment& Fragment::operator=(Fragment&& other) noexcept
    {
        assert(this != &other);

        if (_impl)
        {
            this->~Fragment();
        }

        _impl = std::exchange(other._impl, nullptr);
        _startOffset = std::exchange(other._startOffset, 0);
        _size = std::exchange(other._size, 0);

        return *this;
    }

    Fragment::~Fragment()
    {
        if (_impl)
        {
            if (_impl->header.refCount.fetch_sub(1) == 1)
            {
                _impl->~Impl();

                scalable_free(_impl);
            }

            _impl = nullptr;
        }
    }

    bool Fragment::IsValid() const
    {
        return _impl != nullptr && _size > 0;
    }

    auto Fragment::SliceFront(int64_t size) -> std::optional<Fragment>
    {
        if (size >= GetSize())
        {
            return std::nullopt;
        }

        Fragment result;
        result._impl = _impl;
        result._startOffset = _startOffset;
        result._size = size;

        _startOffset += size;
        _size -= size;

        [[maybe_unused]]
        const int64_t prev = _impl->header.refCount.fetch_add(1);
        assert(prev > 0);

        return result;
    }

    auto Fragment::ShallowCopy() const -> Fragment
    {
        if (!IsValid())
        {
            assert(false);

            return {};
        }

        Fragment result;
        result._impl = _impl;
        result._startOffset = _startOffset;
        result._size = _size;

        [[maybe_unused]]
        const int64_t prev = _impl->header.refCount.fetch_add(1);
        assert(prev > 0);

        return result;
    }

    auto Fragment::GetData() -> char*
    {
        if (!_impl)
        {
            return nullptr;
        }

        return reinterpret_cast<char*>(_impl) + sizeof(Impl) + _startOffset;
    }

    auto Fragment::GetData() const -> const char*
    {
        if (!_impl)
        {
            return nullptr;
        }

        return reinterpret_cast<const char*>(_impl) + sizeof(Impl) + _startOffset;
    }

    auto Fragment::GetSize() const -> int64_t
    {
        return _size;
    }

    auto Fragment::GetSpan() const -> std::span<const char>
    {
        return std::span{ GetData(), static_cast<size_t>(GetSize()) };
    }

    auto Fragment::GetRefCount() const -> int64_t
    {
        if (_impl)
        {
            return _impl->header.refCount.load();
        }

        return 0;
    }

    auto Fragment::CreateFrom(std::span<const char> span) -> Fragment
    {
        Fragment fragment = Create(std::ssize(span));
        std::ranges::copy(span, fragment.GetData());

        return fragment;
    }

    auto Fragment::Create(int64_t size) -> Fragment
    {
        assert(size > 0);

        void* ptr = scalable_malloc(sizeof(Impl) + (sizeof(char) * size));

        Fragment fragment;
        fragment._impl = new (ptr) Impl();
        fragment._impl->header.refCount = 1;
        fragment._impl->header.size = size;

        fragment._startOffset = 0;
        fragment._size = size;

        return fragment;
    }
}
