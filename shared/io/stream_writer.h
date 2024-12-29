#pragma once
#include <cassert>
#include <span>
#include <concepts>
#include <string>
#include <iterator>
#include <algorithm>
#include <optional>
#include <stdexcept>

namespace sunlight
{
    template <typename T>
    concept stream_writable_concept = requires (T t, typename T::value_type v)
    {
        { t.push_back(v) } -> std::same_as<void>;
        { t.begin() };
        { t.size() } -> std::same_as<size_t>;
    };

    template <stream_writable_concept T>
    class StreamWriter
    {
    public:
        StreamWriter() = default;
        StreamWriter(const StreamWriter& other) = delete;
        StreamWriter(StreamWriter&& other) noexcept = delete;
        StreamWriter& operator=(const StreamWriter& other) = delete;
        StreamWriter& operator=(StreamWriter&& other) noexcept = delete;

        explicit StreamWriter(T& container);
        ~StreamWriter() = default;

        template <std::integral U>
        void Write(U value);
        void WriteString(const std::string& str);
        void WriteBuffer(std::span<const char> buffer);

        template <std::integral U>
        void Write(U value, int64_t offset);

        template <std::floating_point U>
        void Write(U value);

        auto GetWriteSize() const ->int64_t;

    protected:
        T& _container;
        int64_t _writeSize = 0;
    };

    template <stream_writable_concept T>
    StreamWriter<T>::StreamWriter(T& container)
        : _container(container)
    {
    }

    template <stream_writable_concept T>
    template <std::integral U>
    void StreamWriter<T>::Write(U value)
    {
        const char* begin = reinterpret_cast<const char*>(&value);
        const char* end = begin + sizeof(value);

        std::copy(begin, end, std::back_inserter(_container));
        _writeSize += sizeof(value);
    }

    template <stream_writable_concept T>
    void StreamWriter<T>::WriteString(const std::string& str)
    {
        std::copy(str.begin(), str.end(), std::back_inserter(_container));
        _container.push_back('\0');

        _writeSize += std::ssize(str) + 1;
    }

    template <stream_writable_concept T>
    void StreamWriter<T>::WriteBuffer(std::span<const char> buffer)
    {
        std::ranges::copy(buffer, std::back_inserter(_container));
        _writeSize += std::ssize(buffer);
    }

    template <stream_writable_concept T>
    template <std::integral U>
    void StreamWriter<T>::Write(U value, int64_t offset)
    {
        const int64_t size = std::ssize(_container);
        if (offset < 0 || offset > size || offset + sizeof(U) > size)
        {
            throw std::runtime_error("fail to write stream. invalid offset");
        }
        else if (offset == size)
        {
            const char* begin = reinterpret_cast<const char*>(&value);
            const char* end = begin + sizeof(value);

            std::copy(begin, end, std::next(_container.begin(), offset));

            return;
        }

        
        const char* begin = reinterpret_cast<const char*>(&value);
        const char* end = begin + sizeof(value);

        std::copy(begin, end, std::next(_container.begin(), offset));
    }

    template <stream_writable_concept T>
    template <std::floating_point U>
    void StreamWriter<T>::Write(U value)
    {
        const char* begin = reinterpret_cast<const char*>(&value);
        const char* end = begin + sizeof(value);

        std::copy(begin, end, std::back_inserter(_container));
        _writeSize += sizeof(value);
    }

    template <stream_writable_concept T>
    auto StreamWriter<T>::GetWriteSize() const -> int64_t
    {
        return _writeSize;
    }
}
