#pragma once
#include "sl/generator/game/entity/game_monster.h"
#include "sl/generator/game/system/game_system.h"

namespace sunlight
{
    class ItemData;
}

namespace sunlight
{
    class MonsterDropItemTableSystem final : public GameSystem
    {
    private:
        enum class DropItemType
        {
            Normal = 0,
            Rare,

            Count
        };

        struct DropItemSet
        {
            int32_t probability = 0;

            int32_t maxWeight = 0;
            std::vector<std::pair<PtrNotNull<const ItemData>, int32_t>> items;
        };

        struct DropItemTable
        {
            std::array<std::vector<std::pair<PtrNotNull<const ItemData>, int32_t>>, static_cast<int32_t>(DropItemType::Count)> itemLists;
            std::array<std::vector<DropItemSet>, static_cast<int32_t>(DropItemType::Count)> itemSetLists;
        };

    public:
        MonsterDropItemTableSystem(const ServiceLocator& serviceLocator, int32_t stageId);

        void InitializeSubSystem(Stage& stage) override;
        bool Subscribe(Stage& stage) override;
        auto GetName() const -> std::string_view override;
        auto GetClassId() const -> game_system_id_type override;

        void GetMonsterDropItem(std::vector<std::pair<PtrNotNull<const ItemData>, int32_t>>& result, const GameMonster& monster);

    private:
        void CreateDropItemTable(const GameMonster& monster);

    private:
        auto RandItemQuantity(DropItemType type, const ItemData& itemData) -> int32_t;
        auto RandItemGenTime(DropItemType type, int32_t frequency) -> std::chrono::milliseconds;

    private:
        const ServiceLocator& _serviceLocator;
        int32_t _stageId = 0;
        std::mt19937 _mt;

        PtrNotNull<const ItemData> _goldItemData = nullptr;
        std::unordered_map<int32_t, DropItemTable> _dropItemTables;
    };
}
