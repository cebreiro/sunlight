#pragma once

namespace sunlight
{
    class DatabaseService;
    class Trie;
}

namespace sunlight
{
    class UniqueNameService final
        : public IService
        , public std::enable_shared_from_this<UniqueNameService>
    {
    public:
        UniqueNameService(const ServiceLocator& serviceLocator, execution::IExecutor& executor);

        auto InitializeName(DatabaseService& databaseService) -> Future<void>;

        auto GetName() const -> std::string_view override;

    public:
        auto Has(std::wstring name) const -> Future<bool>;

        using reserve_key_type = ValueType<int64_t, UniqueNameService>;

        auto Reserve(std::wstring name) -> Future<std::optional<reserve_key_type>>;
        auto Commit(reserve_key_type key) -> Future<void>;
        auto Rollback(reserve_key_type key) -> Future<void>;

    private:
        const ServiceLocator& _serviceLocator;
        SharedPtrNotNull<Strand> _strand;

        int64_t _nextKey = 0;
        std::unordered_map<reserve_key_type, std::wstring> _reserved;

        SharedPtrNotNull<Trie> _trie;
    };
}
