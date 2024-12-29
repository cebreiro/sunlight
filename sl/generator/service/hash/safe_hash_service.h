#pragma once

namespace sunlight
{
    class SafeHashService final
        : public IService
        , public std::enable_shared_from_this<SafeHashService>
    {
    public:
        SafeHashService(const ServiceLocator& serviceLocator, execution::IExecutor& executor);

        auto GetName() const->std::string_view override;

    public:
        auto Hash(std::string pwd) -> Future<std::string>;
        auto Compare(std::string hashed, std::string raw) -> Future<bool>;

    private:
        auto MakeSalt() -> std::array<char, 16>;

    private:
        const ServiceLocator& _serviceLocator;
        SharedPtrNotNull<Strand> _strand;

        std::mt19937 _mt;
    };
}
