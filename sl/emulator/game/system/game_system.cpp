#include "game_system.h"

namespace sunlight
{
    bool GameSystem::HasCyclicSystemDependency() const
    {
        const game_system_id_type selfId = this->GetClassId();
        const auto range = _systems | std::views::values
            | std::views::transform([](const SharedPtrNotNull<GameSystem>& system) -> PtrNotNull<const GameSystem>
                {
                    return system.get();
                });

        std::vector<PtrNotNull<const GameSystem>> stack = std::ranges::to<std::vector>(range);
        std::unordered_set<PtrNotNull<const GameSystem>> visits;

        while (!stack.empty())
        {
            PtrNotNull<const GameSystem> current = stack.back();
            stack.pop_back();

            if (!visits.emplace(current).second)
            {
                continue;
            }

            if (current->GetClassId() == selfId)
            {
                return true;
            }

            for (const SharedPtrNotNull<GameSystem>& child : current->_systems | std::views::values)
            {
                stack.push_back(child.get());
            }
        }

        return false;
    }

    void GameSystem::InitializeSubSystem(Stage& stage)
    {
        (void)stage;
    }

    bool GameSystem::Subscribe(Stage& stage)
    {
        (void)stage;

        return true;
    }
}
