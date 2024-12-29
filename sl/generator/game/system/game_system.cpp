#include "game_system.h"

namespace sunlight
{
    bool GameSystem::HasCyclicSystemDependency(std::vector<std::string>* optResultCyclePath) const
    {
        const game_system_id_type selfId = this->GetClassId();
        const auto range = _systems | std::views::values;

        std::vector<PtrNotNull<GameSystem>> stack = std::ranges::to<std::vector>(range);
        std::unordered_set<PtrNotNull<GameSystem>> visits;
        std::unordered_map<PtrNotNull<GameSystem>, PtrNotNull<GameSystem>> paths;

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
                if (optResultCyclePath)
                {
                    PtrNotNull<GameSystem> system = current;
                    while (system)
                    {
                        optResultCyclePath->emplace_back(system->GetName());

                        const auto iter = paths.find(system);

                        system = iter != paths.end() ? iter->second : nullptr;
                    }
                }

                return true;
            }

            for (const PtrNotNull<GameSystem>& child : current->_systems | std::views::values)
            {
                paths[child] = current;

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
