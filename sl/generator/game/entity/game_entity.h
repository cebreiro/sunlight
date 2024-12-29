#pragma once
#include "sl/generator/game/entity/game_entity_type.h"
#include "sl/generator/game/entity/game_entity_id_type.h"
#include "sl/generator/game/component/game_component.h"

namespace sunlight
{
    class GameEntityIdPublisher;
}

namespace sunlight
{
    class GameEntity
    {
    public:
        GameEntity() = delete;
        GameEntity(const GameEntity& other) = delete;
        GameEntity(GameEntity&& other) noexcept = delete;
        GameEntity& operator=(const GameEntity& other) = delete;
        GameEntity& operator=(GameEntity&& other) noexcept = delete;

    public:
        GameEntity(GameEntityIdPublisher& idPublisher, GameEntityType type);
        GameEntity(game_entity_id_type id, GameEntityType type);
        virtual ~GameEntity();

        bool IsActive() const;

        auto GetId() const -> game_entity_id_type;
        auto GetType() const -> GameEntityType;

        void SetActive(bool value);

    public:
        template <typename T> requires std::derived_from<T, GameEntity>
        auto Cast() -> T*;

        template <typename T> requires std::derived_from<T, GameEntity>
        auto Cast() const -> const T*;

    public:
        template <typename T> requires std::derived_from<T, GameComponent>
        bool HasComponent() const;

        template <typename T> requires std::derived_from<T, GameComponent>
        bool AddComponent(UniquePtrNotNull<T> component);

        template <typename T> requires std::derived_from<T, GameComponent>
        bool RemoveComponent();

        template <typename T> requires std::derived_from<T, GameComponent>
        auto FindComponent() -> T*;

        template <typename T> requires std::derived_from<T, GameComponent>
        auto FindComponent() const -> const T*;

        template <typename T> requires std::derived_from<T, GameComponent>
        bool FindAndThen(const std::function<void(T&)>& func);

        template <typename T> requires std::derived_from<T, GameComponent>
        bool FindAndThen(const std::function<void(const T&)>& func) const;

        template <typename T> requires std::derived_from<T, GameComponent>
        auto GetComponent() -> T&;

        template <typename T> requires std::derived_from<T, GameComponent>
        auto GetComponent() const -> const T&;

    private:
        GameEntityIdPublisher* _idPublisher = nullptr;

        game_entity_id_type _id = {};
        GameEntityType _type = GameEntityType::None;
        bool _active = false;

        std::unordered_map<game_component_id_type, UniquePtrNotNull<GameComponent>> _components;
    };

    template <typename T> requires std::derived_from<T, GameEntity>
    auto GameEntity::Cast() -> T*
    {
        if (_type == T::TYPE)
        {
            T* result = static_cast<T*>(this);
            assert(result == dynamic_cast<T*>(this));

            return result;
        }

        return nullptr;
    }

    template <typename T> requires std::derived_from<T, GameEntity>
    auto GameEntity::Cast() const -> const T*
    {
        if (_type == T::TYPE)
        {
            const T* result = static_cast<const T*>(this);
            assert(result == dynamic_cast<const T*>(this));

            return result;
        }

        return nullptr;
    }

    template <typename T> requires std::derived_from<T, GameComponent>
    bool GameEntity::HasComponent() const
    {
        return FindComponent<T>() != nullptr;
    }

    template <typename T> requires std::derived_from<T, GameComponent>
    bool GameEntity::AddComponent(UniquePtrNotNull<T> component)
    {
        assert(component);

        const auto& id = GameComponent::GetClassId<T>();

        const auto& [iter, inserted] = _components.try_emplace(id, nullptr);
        if (inserted)
        {
            iter->second = std::move(component);
        }

        return inserted;
    }

    template <typename T> requires std::derived_from<T, GameComponent>
    bool GameEntity::RemoveComponent()
    {
        const auto& id = GameComponent::GetClassId<T>();

        return _components.erase(id);
    }

    template <typename T> requires std::derived_from<T, GameComponent>
    auto GameEntity::FindComponent() -> T*
    {
        const auto& id = GameComponent::GetClassId<T>();

        const auto iter = _components.find(id);
        if (iter != _components.end())
        {
            assert(iter->second);

            T* result = static_cast<T*>(iter->second.get());
            assert(result == dynamic_cast<T*>(iter->second.get()));

            return result;
        }

        return nullptr;
    }

    template <typename T> requires std::derived_from<T, GameComponent>
    auto GameEntity::FindComponent() const -> const T*
    {
        const auto& id = GameComponent::GetClassId<T>();

        const auto iter = _components.find(id);
        if (iter != _components.end())
        {
            assert(iter->second);

            const T* result = static_cast<const T*>(iter->second.get());
            assert(result == dynamic_cast<const T*>(iter->second.get()));

            return result;
        }

        return nullptr;
    }

    template <typename T> requires std::derived_from<T, GameComponent>
    bool GameEntity::FindAndThen(const std::function<void(T&)>& func)
    {
        assert(func);

        if (T* component = FindComponent<T>(); component)
        {
            func(*component);

            return true;
        }

        return false;
    }

    template <typename T> requires std::derived_from<T, GameComponent>
    bool GameEntity::FindAndThen(const std::function<void(const T&)>& func) const
    {
        assert(func);

        if (const T* component = FindComponent<T>(); component)
        {
            func(*component);

            return true;
        }

        return false;
    }

    template <typename T> requires std::derived_from<T, GameComponent>
    auto GameEntity::GetComponent() -> T&
    {
        T* component = FindComponent<T>();
        assert(component);

        return *component;
    }

    template <typename T> requires std::derived_from<T, GameComponent>
    auto GameEntity::GetComponent() const -> const T&
    {
        const T* component = FindComponent<T>();
        assert(component);

        return *component;
    }
}
