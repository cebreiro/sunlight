#pragma once
#include "sl/generator/game/system/game_system.h"

namespace sunlight
{
    // used for bubbling events to higher-level systems
    // be cautious to avoid logical errors and stack overflow crashes caused by cyclic dependencies
    class EventBubblingSystem final : public GameSystem
    {
    public:
        void InitializeSubSystem(Stage& stage) override;
        bool Subscribe(Stage& stage) override;
        auto GetName() const -> std::string_view override;
        auto GetClassId() const -> game_system_id_type override;

    public:
        void Publish(const std::any& any);

        template <typename T>
        void AddSubscriber(const std::function<void(const T&)>& handler);

    private:
        std::unordered_multimap<std::type_index, std::function<void(const std::any&)>> _subscribers;
    };

    template <typename T>
    void EventBubblingSystem::AddSubscriber(const std::function<void(const T&)>& handler)
    {
        _subscribers.emplace(std::type_index(typeid(T)), [handler](const std::any& any)
            {
                handler(std::any_cast<T>(any));
            });
    }
}
