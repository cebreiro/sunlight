#pragma once
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <google/protobuf/io/coded_stream.h>

namespace sunlight
{
    class ZeroCopyInputStream final : public google::protobuf::io::ZeroCopyInputStream
    {
    public:
        explicit ZeroCopyInputStream(const Buffer& buffer);

        bool Next(const void** data, int32_t* size) override;
        void BackUp(int32_t count) override;

        bool Skip(int32_t count) override;

        auto ByteCount() const -> int64_t override;

    private:
        const Buffer& _buffer;
        int32_t _containerIndex = -1;
        int32_t _bufferIndex = 0;
    };
}
