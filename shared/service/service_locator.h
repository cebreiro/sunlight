#pragma once
#include <memory>
#include <boost/container/flat_map.hpp>
#include "shared/service/service_interface.h"
#include "shared/type/not_null_pointer.h"
#include "shared/type/runtime_type_id.h"

namespace sunlight
{
    class ServiceLocator
    {
        using service_id_type = RuntimeTypeId<IService>;

    public:
        ServiceLocator(const ServiceLocator&) = default;
        ServiceLocator& operator=(const ServiceLocator&) = default;
        ServiceLocator() = default;

        template <std::derived_from<IService> T>
        bool Add(SharedPtrNotNull<IService> service);

        template <std::derived_from<IService> T>
        bool Remove();

        template <std::derived_from<IService> T>
        auto Find() -> T*;

        template <std::derived_from<IService> T>
        auto Find() const -> T*;

        template <std::derived_from<IService> T>
        auto Get() -> T&;

        template <std::derived_from<IService> T>
        auto Get() const -> T&;

        template <std::derived_from<IService> T>
        auto FindShared() -> SharedPtrNotNull<T>;

        template <std::derived_from<IService> T>
        auto FindShared() const -> SharedPtrNotNull<T>;

    private:
        boost::container::flat_map<int64_t, SharedPtrNotNull<IService>> _services;
    };

    template <std::derived_from<IService> T>
    bool ServiceLocator::Add(SharedPtrNotNull<IService> service)
    {
        const int64_t id = service_id_type::Get<T>();
        assert(dynamic_cast<const T*>(service.get()) != nullptr);

        return _services.try_emplace(id, std::move(service)).second;
    }

    template <std::derived_from<IService> T>
    bool ServiceLocator::Remove()
    {
        const int64_t id = service_id_type::Get<T>();

        return _services.erase(id);
    }

    template <std::derived_from<IService> T>
    auto ServiceLocator::Find() -> T*
    {
        const int64_t id = service_id_type::Get<T>();
        auto iter = _services.find(id);

        if (iter != _services.end())
        {
            auto result = static_cast<T*>(iter->second.get());
            assert(result == dynamic_cast<T*>(iter->second.get()));

            return result;
        }

        return nullptr;
    }

    template <std::derived_from<IService> T>
    auto ServiceLocator::Find() const -> T*
    {
        const int64_t id = service_id_type::Get<T>();
        auto iter = _services.find(id);

        if (iter != _services.end())
        {
            auto result = static_cast<T*>(iter->second.get());
            assert(result == dynamic_cast<T*>(iter->second.get()));

            return result;
        }

        return nullptr;
    }

    template <std::derived_from<IService> T>
    auto ServiceLocator::Get() -> T&
    {
        T* service = Find<T>();
        assert(service);

        return *service;
    }

    template <std::derived_from<IService> T>
    auto ServiceLocator::Get() const -> T&
    {
        T* service = Find<T>();
        assert(service);

        return *service;
    }

    template <std::derived_from<IService> T>
    auto ServiceLocator::FindShared() -> SharedPtrNotNull<T>
    {
        const int64_t id = service_id_type::Get<T>();
        auto iter = _services.find(id);

        if (iter != _services.end())
        {
            auto result = std::static_pointer_cast<T>(iter->second);
            assert(result == std::dynamic_pointer_cast<T>(iter->second));

            return result;
        }

        return nullptr;
    }

    template <std::derived_from<IService> T>
    auto ServiceLocator::FindShared() const -> SharedPtrNotNull<T>
    {
        const int64_t id = service_id_type::Get<T>();
        auto iter = _services.find(id);

        if (iter != _services.end())
        {
            auto result = std::static_pointer_cast<T>(iter->second);
            assert(result == std::dynamic_pointer_cast<T>(iter->second));

            return result;
        }

        return nullptr;
    }

    template <typename... TServices> requires std::conjunction_v<std::is_base_of<IService, TServices>...>
    class ServiceLocatorT
    {
    public:
        template <typename T, typename... TArgs>
        using is_one_of = std::disjunction<std::is_same<T, TArgs>...>;

    public:
        ServiceLocatorT() = default;
        explicit(false) ServiceLocatorT(ServiceLocator& serviceLocator)
            : _tuple({ serviceLocator.FindShared<TServices>()... })
        {
        }

        template <typename ...UServices>
            requires std::conjunction_v<is_one_of<TServices, UServices...>...>
        explicit(false) ServiceLocatorT(ServiceLocatorT<UServices...>& serviceLocator)
            : _tuple({ serviceLocator.template FindShared<TServices>()... })
        {
        }

        bool ContainsAll() const
        {
            constexpr auto contains = []<typename T, size_t... I>(const T& tuple, std::index_sequence<I...>)
            {
                return (... & (std::get<I>(tuple) != nullptr));
            };

            return contains(_tuple, std::make_index_sequence<std::tuple_size_v<decltype(_tuple)>>{});
        }

        template <typename U> requires is_one_of<U, TServices...>::value
        bool Contains() const
        {
            return std::get<std::shared_ptr<U>>(_tuple) != nullptr;
        }

        template <typename U> requires is_one_of<U, TServices...>::value
        auto Find() -> U*
        {
            return std::get<std::shared_ptr<U>>(_tuple).get();
        }

        template <typename U> requires is_one_of<U, TServices...>::value
        auto Find() const -> const U*
        {
            return std::get<std::shared_ptr<U>>(_tuple).get();
        }

        template <typename U> requires is_one_of<U, TServices...>::value
        auto Get() -> U&
        {
            auto ptr = std::get<std::shared_ptr<U>>(_tuple);
            assert(ptr);

            return *ptr;
        }

        template <typename U> requires is_one_of<U, TServices...>::value
        auto Get() const -> const U&
        {
            auto ptr = std::get<std::shared_ptr<U>>(_tuple);
            assert(ptr);

            return *ptr;
        }

        template <typename U> requires is_one_of<U, TServices...>::value
        auto FindShared() -> std::shared_ptr<U>
        {
            return std::get<std::shared_ptr<U>>(_tuple);
        }

    private:
        std::tuple<std::shared_ptr<TServices>...> _tuple;
    };
}
