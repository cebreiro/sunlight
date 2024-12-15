#pragma once

namespace sunlight
{
    class Sha256HashService final
        : public IService
        , public std::enable_shared_from_this<Sha256HashService>
    {
    public:
        Sha256HashService(const ServiceLocator& serviceLocator, execution::IExecutor& executor);
        ~Sha256HashService();

        auto GetName() const -> std::string_view override;

    public:
        auto Hash(const char* buffer, size_t length) -> Future<std::string>;

    private:
        const ServiceLocator& _serviceLocator;
        SharedPtrNotNull<Strand> _strand;

        void* evpContext = nullptr;
    };
}
