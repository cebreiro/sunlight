#include "unique_name_service.h"

#include "boost/locale.hpp"
#include "sl/generator/server/server_constant.h"
#include "sl/generator/service/database/database_service.h"

#include "sl/generator/service/unique_name/trie.h"

namespace sunlight
{
    UniqueNameService::UniqueNameService(const ServiceLocator& serviceLocator, execution::IExecutor& executor)
        : _serviceLocator(serviceLocator)
        , _strand(std::make_shared<Strand>(executor.SharedFromThis()))
        , _trie(std::make_shared<Trie>())
    {
    }

    auto UniqueNameService::InitializeName(DatabaseService& databaseService) -> Future<void>
    {
        std::vector<std::string> names = co_await databaseService.GetCharacterNamesAll();

        for (const std::string& name : names)
        {
            const std::wstring& str = boost::locale::conv::to_utf<wchar_t>(name, ServerConstant::TEXT_ENCODING);

            if (!_trie->Insert(str))
            {
                SUNLIGHT_LOG_CRITICAL(_serviceLocator,
                    fmt::format("[{}] trie data structrue has critical error. insert fail name: {}",
                        GetName(), boost::locale::conv::from_utf(str, ServerConstant::TEXT_ENCODING)));
            }
        }
    }

    auto UniqueNameService::GetName() const -> std::string_view
    {
        return "unique_name_service";
    }

    auto UniqueNameService::Has(std::wstring name) const -> Future<bool>
    {
        [[maybe_unused]]
        const auto self = shared_from_this();

        co_await *_strand;
        assert(ExecutionContext::IsEqualTo(*_strand));

        co_return _trie->Contains(name);
    }

    auto UniqueNameService::Reserve(std::wstring name) -> Future<std::optional<reserve_key_type>>
    {
        [[maybe_unused]]
        const auto self = shared_from_this();

        co_await *_strand;
        assert(ExecutionContext::IsEqualTo(*_strand));

        if (_trie->Contains(name))
        {
            co_return std::nullopt;
        }

        if (!_trie->Insert(name))
        {
            SUNLIGHT_LOG_CRITICAL(_serviceLocator,
                fmt::format("[{}] trie data structrue has critical error. insert fail name: {}",
                    GetName(), boost::locale::conv::from_utf(name, ServerConstant::TEXT_ENCODING)));

            co_return std::nullopt;
        }

        const reserve_key_type key(++_nextKey);

        _reserved[key] = std::move(name);

        co_return key;
    }

    auto UniqueNameService::Commit(reserve_key_type key) -> Future<void>
    {
        [[maybe_unused]]
        const auto self = shared_from_this();

        co_await *_strand;
        assert(ExecutionContext::IsEqualTo(*_strand));

        if (const auto iter = _reserved.find(key); iter == _reserved.end())
        {
            SUNLIGHT_LOG_ERROR(_serviceLocator,
                fmt::format("[{}] fail to find name key",
                    GetName()));
        }

        co_return;
    }

    auto UniqueNameService::Rollback(reserve_key_type key) -> Future<void>
    {
        [[maybe_unused]]
        const auto self = shared_from_this();

        co_await *_strand;
        assert(ExecutionContext::IsEqualTo(*_strand));

        const auto iter = _reserved.find(key);
        if (iter == _reserved.end())
        {
            SUNLIGHT_LOG_ERROR(_serviceLocator,
                fmt::format("[{}] fail to find name key",
                    GetName()));

            co_return;
        }

        const std::wstring& name = iter->second;

        if (!_trie->Remove(name))
        {
            SUNLIGHT_LOG_CRITICAL(_serviceLocator,
                fmt::format("[{}] trie data structrue has critical error. remove fail name: {}",
                    GetName(), boost::locale::conv::from_utf(name, ServerConstant::TEXT_ENCODING)));
        }

        _reserved.erase(iter);

        co_return;
    }
}
