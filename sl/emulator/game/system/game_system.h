#pragma once
#include "shared/type/runtime_type_id.h"
#include "sl/emulator/game/system/game_system_id.h"

namespace sunlight
{
    class Stage;
}

namespace sunlight
{
    class GameSystem
    {
    public:
        virtual ~GameSystem() = default;

        bool HasCyclicSystemDependency() const;

        virtual void InitializeSubSystem(Stage& stage);
        virtual bool Subscribe(Stage& stage);

        virtual auto GetClassId() const -> game_system_id_type = 0;

    public:
        template <typename T> requires std::derived_from<T, GameSystem>
        bool Add(SharedPtrNotNull<T> system);

        template <typename T> requires std::derived_from<T, GameSystem>
        auto Find() -> T*;

        template <typename T> requires std::derived_from<T, GameSystem>
        auto Find() const -> const T*;

        template <typename T> requires std::derived_from<T, GameSystem>
        auto Get() -> T&;

        template <typename T> requires std::derived_from<T, GameSystem>
        auto Get() const -> const T&;

    public:
        template <typename T> requires std::derived_from<T, GameSystem>
        static auto GetClassId() -> game_system_id_type;

    private:
        std::unordered_map<game_system_id_type, SharedPtrNotNull<GameSystem>> _systems;
    };

    template <typename T> requires std::derived_from<T, GameSystem>
    bool GameSystem::Add(SharedPtrNotNull<T> system)
    {
        assert(system);

        const auto& id = GameSystem::GetClassId<T>();

        const auto& [iter, inserted] = _systems.try_emplace(id, nullptr);
        if (inserted)
        {
            iter->second = std::move(_systems);
        }

        return inserted;
    }

    template <typename T> requires std::derived_from<T, GameSystem>
    auto GameSystem::Find() -> T*
    {
        const auto& id = GameSystem::GetClassId<T>();

        const auto iter = _systems.find(id);
        if (iter != _systems.end())
        {
            assert(iter->second);

            T* result = static_cast<T*>(iter->second.get());
            assert(result == dynamic_cast<T*>(iter->second.get()));

            return result;
        }

        return nullptr;
    }

    template <typename T> requires std::derived_from<T, GameSystem>
    auto GameSystem::Find() const -> const T*
    {
        const auto& id = GameSystem::GetClassId<T>();

        const auto iter = _systems.find(id);
        if (iter != _systems.end())
        {
            assert(iter->second);

            const T* result = static_cast<const T*>(iter->second.get());
            assert(result == dynamic_cast<const T*>(iter->second.get()));

            return result;
        }

        return nullptr;
    }

    template <typename T> requires std::derived_from<T, GameSystem>
    auto GameSystem::Get() -> T&
    {
        T* system = Find<T>();
        assert(system);

        return *system;
    }

    template <typename T> requires std::derived_from<T, GameSystem>
    auto GameSystem::Get() const -> const T&
    {
        const T* system = Find<T>();
        assert(system);

        return *system;
    }

    template <typename T> requires std::derived_from<T, GameSystem>
    auto GameSystem::GetClassId() -> game_system_id_type
    {
        const int64_t value = RuntimeTypeId<GameSystem>::Get<T>();

        using value_type = game_system_id_type::value_type;
        static_assert(std::is_same_v<int64_t, value_type>);

        return game_system_id_type(value);
    }
}
