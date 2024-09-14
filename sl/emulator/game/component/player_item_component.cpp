#include "player_item_component.h"

#include "sl/emulator/game/component/item_position_component.h"
#include "sl/emulator/game/contants/item/item_slot_stroage.h"
#include "sl/emulator/game/entity/game_entity_id_pool.h"
#include "sl/emulator/game/entity/game_item.h"
#include "sl/emulator/service/gamedata/item/item_data_provider.h"

namespace sunlight
{
    PlayerItemComponent::PlayerItemComponent(GameEntityIdPool& idPool, const ItemDataProvider& itemDataProvider,
        const db::dto::Character& dto)
        : _gold(dto.gold)
        , _inventoryPage(std::clamp<int32_t>(dto.inventoryPage, 1, GameConstant::MAX_INVENTORY_PAGE_SIZE))
    {
        for (int64_t i = 0; i < std::ssize(_inventory); ++i)
        {
            _inventory[i] = std::make_unique<ItemSlotStorage>(GameConstant::INVENTORY_WIDTH, GameConstant::INVENTORY_HEIGHT);
        }

        for (const db::dto::Character::Item& dtoItem : dto.items)
        {
            const ItemData* data = itemDataProvider.Find(dtoItem.dataId);
            if (!data)
            {
                throw std::runtime_error(fmt::format("fail to find item data. cid: {}, item_id: {}",
                    dto.id, dtoItem.dataId));
            }

            const game_entity_id_type itemEntityId = idPool.Pop(GameEntityType::Item);

            SharedPtrNotNull<GameItem>& itemEntity = _items[itemEntityId];
            assert(!itemEntity);

            itemEntity = std::make_shared<GameItem>(itemEntityId, *data, dtoItem.quantity);
            itemEntity->SetUId(dtoItem.id);

            auto positionComponent = new ItemPositionComponent();
            positionComponent->SetPage(dtoItem.page);
            positionComponent->SetX(dtoItem.x);
            positionComponent->SetY(dtoItem.y);

            (void)itemEntity->AddComponent(std::unique_ptr<ItemPositionComponent>(positionComponent));

            switch (dtoItem.posType)
            {
            case db::ItemPosType::Inventory:
            {
                positionComponent->SetPositionType(ItemPositionType::Inventory);

                if (positionComponent->GetPage() >= std::ssize(_inventory))
                {
                    assert(false);

                    continue;
                }

                ItemSlotStorage& storage = *_inventory[positionComponent->GetPage()];

                const ItemSlotRange range{
                    .x = positionComponent->GetX(),
                    .y = positionComponent->GetY(),
                    .xSize = itemEntity->GetData().GetWidth(),
                    .ySize = itemEntity->GetData().GetHeight(),
                };

                if (!storage.Contains(range))
                {
                    throw std::runtime_error(fmt::format("invalid item slot. cid: {}, item_id: {}, range: [X: {}, y: {}, x_size: {}, y_size: {}]",
                        dto.id, dtoItem.id, range.x, range.y, range.xSize, range.ySize));
                }

                if (!storage.HasEmptySlot(range))
                {
                    throw std::runtime_error(fmt::format("item overlapped. cid: {}, item_id: {}, range: [X: {}, y: {}, x_size: {}, y_size: {}]",
                        dto.id, dtoItem.id, range.x, range.y, range.xSize, range.ySize));
                }

                storage.Set(itemEntity.get(), range);
            }
            break;
            case db::ItemPosType::Equipment:
            {
                positionComponent->SetPositionType(ItemPositionType::Equipment);

                const int64_t index = positionComponent->GetPage();
                if (index < 0 || index >= std::ssize(_equipments))
                {
                    throw std::runtime_error(fmt::format("invalid equipment slot. cid: {}, item_id: {}, slot: {}",
                        dto.id, dtoItem.id, index));
                }

                GameItem*& equipment = _equipments[index];
                if (equipment)
                {
                    throw std::runtime_error(fmt::format("duplicated equipment item. cid: {}, item_id: {}, slot: {}",
                        dto.id, dtoItem.id, index));
                }

                equipment = itemEntity.get();
            }
            break;
            case db::ItemPosType::QuickSlot:
            {
                positionComponent->SetPositionType(ItemPositionType::QuickSlot);
            }
            break;
            case db::ItemPosType::Pick:
            {
                positionComponent->SetPositionType(ItemPositionType::Pick);

                if (_pickItem)
                {
                    throw std::runtime_error(fmt::format("duplicated pick item. cid: {}, item_id: {}, already_picked_item_id: {}",
                        dto.id, dtoItem.id, _pickItem->GetUId().value_or(-1)));
                }

                _pickItem = itemEntity.get();
            }
            break;
            }
        }
    }

    PlayerItemComponent::~PlayerItemComponent()
    {
    }

    auto PlayerItemComponent::GetGold() const -> int32_t
    {
        return _gold;
    }

    auto PlayerItemComponent::GetInventoryPage() const -> int32_t
    {
        return _inventoryPage;
    }

    auto PlayerItemComponent::GetPickedItem() const -> const GameItem*
    {
        return _pickItem;
    }
}
