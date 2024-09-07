#pragma once
#include <cassert>
#include <concepts>
#include "shared/type/not_null_pointer.h"

namespace sunlight
{
    template <typename T>
    concept borrowed_value_concept = requires (T t)
    {
        requires std::move_constructible<T>;
    };

    template <typename T, typename U>
    concept value_owner_concept = requires (T t, U & u)
    {
        requires borrowed_value_concept<U>;
        { t.TakeBack(u) } -> std::same_as<void>;
    };

    template <borrowed_value_concept T, typename U>
    class Borrowed
    {
    public:
        Borrowed(const Borrowed& other) = delete;
        Borrowed& operator=(const Borrowed& other) = delete;

        Borrowed() = default;
        Borrowed(Borrowed&& other) noexcept;
        Borrowed& operator=(Borrowed&& other) noexcept;

        Borrowed(T& item, SharedPtrNotNull<U> owner);
        ~Borrowed();

        bool IsValid() const;
        void Reset();

        auto operator*() -> T&;
        auto operator*() const -> const T&;
        auto operator->() -> T*;
        auto operator->() const -> const T*;

    private:
        T* _item = nullptr;
        SharedPtrNotNull<U> _owner; 
    };

    template <borrowed_value_concept T, typename U> 
    Borrowed<T, U>::Borrowed(Borrowed&& other) noexcept
        : _item(std::exchange(other._item, nullptr))
        , _owner(std::move(other._owner))
    {
        static_assert(value_owner_concept<U, T>);
    }

    template <borrowed_value_concept T, typename U> 
    Borrowed<T, U>& Borrowed<T, U>::operator=(Borrowed&& other) noexcept
    {
        if (IsValid())
        {
            Reset();
        }

        _item = std::exchange(other._item, nullptr);
        _owner = std::move(other._owner);

        return *this;
    }

    template <borrowed_value_concept T, typename U> 
    Borrowed<T, U>::Borrowed(T& item, SharedPtrNotNull<U> owner)
        : _item(&item)
        , _owner(std::move(owner))
    {
        assert(_item != nullptr);
        assert(_owner.operator bool());
    }

    template <borrowed_value_concept T, typename U> 
    Borrowed<T, U>::~Borrowed()
    {
        Reset();
    }

    template <borrowed_value_concept T, typename U> 
    bool Borrowed<T, U>::IsValid() const
    {
        return _item;
    }

    template <borrowed_value_concept T, typename U>
    void Borrowed<T, U>::Reset()
    {
        if (!IsValid())
        {
            return;
        }

        _owner->TakeBack(*_item);
        _owner.reset();
    }

    template <borrowed_value_concept T, typename U> 
    auto Borrowed<T, U>::operator*() -> T&
    {
        assert(IsValid());
        return *_item;
    }

    template <borrowed_value_concept T, typename U> 
    auto Borrowed<T, U>::operator*() const -> const T&
    {
        assert(IsValid());
        return *_item;
    }

    template <borrowed_value_concept T, typename U> 
    auto Borrowed<T, U>::operator->() -> T*
    {
        assert(IsValid());
        return _item;
    }

    template <borrowed_value_concept T, typename U> 
    auto Borrowed<T, U>::operator->() const -> const T*
    {
        assert(IsValid());
        return _item;
    }
}
