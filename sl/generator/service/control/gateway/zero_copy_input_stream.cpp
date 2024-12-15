#include "zero_copy_input_stream.h"

namespace sunlight
{
    ZeroCopyInputStream::ZeroCopyInputStream(const Buffer& buffer)
        : _buffer(buffer)
        , _containerIndex(0)
    {
    }

    bool ZeroCopyInputStream::Next(const void** data, int32_t* size)
    {
        const Buffer::fragment_container_type& fragmentContainer = _buffer.GetFragmentContainer();

        if (_containerIndex == std::ssize(fragmentContainer))
        {
            return false;
        }

        assert(_containerIndex >= 0 && _containerIndex < std::ssize(fragmentContainer));

        const buffer::Fragment& fragment = fragmentContainer[_containerIndex];

        *data = fragment.GetData();
        *size = static_cast<int32_t>(fragment.GetSize());

        ++_containerIndex;
        _bufferIndex = 0;

        return true;
    }

    void ZeroCopyInputStream::BackUp(int32_t count)
    {
        _bufferIndex -= count;

        while (_bufferIndex < 0 && _containerIndex > 0)
        {
            --_containerIndex;
            assert(_containerIndex >= 0);

            _bufferIndex += static_cast<int32_t>(_buffer.GetFragmentContainer()[_containerIndex].GetSize());
        }

        assert(_containerIndex >= 0);
        assert(_bufferIndex >= 0);
    }

    bool ZeroCopyInputStream::Skip(int32_t count)
    {
        const Buffer::fragment_container_type& fragmentContainer = _buffer.GetFragmentContainer();

        int32_t remain = count;

        while (remain > 0 && _containerIndex < std::ssize(fragmentContainer))
        {
            const buffer::Fragment& fragment = fragmentContainer[_containerIndex];

            const int32_t available = static_cast<int32_t>(fragment.GetSize()) - _bufferIndex;

            if (remain <= available)
            {
                _bufferIndex += remain;

                return true;
            }

            remain -= available;

            ++_containerIndex;
            _bufferIndex = 0;
        }

        return remain == 0;
    }

    auto ZeroCopyInputStream::ByteCount() const -> int64_t
    {
        const Buffer::fragment_container_type& fragmentContainer = _buffer.GetFragmentContainer();

        int64_t count = 0;

        for (int32_t i = 0; i < _containerIndex; ++i)
        {
            count += fragmentContainer[i].GetSize();
        }

        count += _bufferIndex;

        return count;
    }
}
