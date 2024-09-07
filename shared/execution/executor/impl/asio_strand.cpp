#include "asio_strand.h"

namespace sunlight::execution
{
    AsioStrand::AsioStrand(boost::asio::strand<boost::asio::io_context::executor_type> strand)
        : _strand(std::move(strand))
    {
    }

    AsioStrand::~AsioStrand()
    {
    }

    void AsioStrand::Stop()
    {
    }

    void AsioStrand::Post(const std::function<void()>& function)
    {
        post(_strand, function);
    }

    void AsioStrand::Post(std::move_only_function<void()> function)
    {
        post(_strand, std::move(function));
    }

    void AsioStrand::Dispatch(const std::function<void()>& function)
    {
        dispatch(_strand, function);
    }

    void AsioStrand::Dispatch(std::move_only_function<void()> function)
    {
        dispatch(_strand, std::move(function));
    }

    auto AsioStrand::GetConcurrency() const -> int64_t
    {
        return 1;
    }

    auto AsioStrand::SharedFromThis() -> SharedPtrNotNull<IExecutor>
    {
        return shared_from_this();
    }

    auto AsioStrand::SharedFromThis() const -> SharedPtrNotNull<const IExecutor>
    {
        return shared_from_this();
    }

    auto AsioStrand::GetImpl() -> boost::asio::strand<boost::asio::io_context::executor_type>&
    {
        return _strand;
    }

    auto AsioStrand::GetImpl() const -> const boost::asio::strand<boost::asio::io_context::executor_type>&
    {
        return _strand;
    }
}
