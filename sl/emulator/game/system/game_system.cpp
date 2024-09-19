#include "game_system.h"

namespace sunlight
{
    bool GameSystem::HasCyclicSystemDependency() const
    {
        const game_system_id_type selfId = this->GetClassId();
        const auto range = _systems | std::views::values;

        std::vector<PtrNotNull<GameSystem>> stack = std::ranges::to<std::vector>(range);
        std::unordered_set<PtrNotNull<GameSystem>> visits;

        while (!stack.empty())
        {
            PtrNotNull<GameSystem> current = stack.back();
            stack.pop_back();

            if (!visits.emplace(current).second)
            {
                continue;
            }

            if (current->GetClassId() == selfId)
            {
                return true;
            }

            for (const PtrNotNull<GameSystem>& child : current->_systems | std::views::values)
            {
                stack.push_back(child);
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

    bool GameSystem::ShouldUpdate() const
    {
        return false;
    }

    void GameSystem::Update()
    {
    }
}
