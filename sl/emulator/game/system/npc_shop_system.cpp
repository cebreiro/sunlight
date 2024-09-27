#include "npc_shop_system.h"

#include <boost/unordered/unordered_flat_set.hpp>

#include "sl/emulator/game/component/item_position_component.h"
#include "sl/emulator/game/component/npc_item_shop_component.h"
#include "sl/emulator/game/entity/game_item.h"
#include "sl/emulator/game/entity/game_npc.h"
#include "sl/emulator/game/entity/game_player.h"
#include "sl/emulator/game/message/zone_message.h"
#include "sl/emulator/game/message/creator/npc_message_creator.h"
#include "sl/emulator/game/system/item_archive_system.h"
#include "sl/emulator/game/zone/stage.h"
#include "sl/emulator/game/zone/service/game_entity_id_publisher.h"
#include "sl/emulator/service/gamedata/gamedata_provide_service.h"
#include "sl/emulator/service/gamedata/item/item_data_provider.h"
#include "sl/emulator/service/gamedata/shop/item_shop_data.h"

namespace sunlight
{
    NPCShopSystem::NPCShopSystem(const ServiceLocator& serviceLocator)
        : _serviceLocator(serviceLocator)
        , _mt19937(std::random_device{}())
    {
    }

    void NPCShopSystem::InitializeSubSystem(Stage& stage)
    {
        Add(stage.Get<ItemArchiveSystem>());
    }

    bool NPCShopSystem::Subscribe(Stage& stage)
    {
        (void)stage;

        return true;
    }

    auto NPCShopSystem::GetName() const -> std::string_view
    {
        return "npc_shop_system";
    }

    auto NPCShopSystem::GetClassId() const -> game_system_id_type
    {
        return GameSystem::GetClassId<NPCShopSystem>();
    }

    void NPCShopSystem::InitializeItemShop(GameNPC& npc)
    {
        const ItemDataProvider& itemDataProvider = _serviceLocator.Get<GameDataProvideService>().GetItemDataProvider();

        NPCItemShopComponent& itemShopComponent = npc.GetComponent<NPCItemShopComponent>();

        std::unordered_map<int32_t, std::unordered_map<int32_t, NPCShopItemBucket>> temp;

        for (int32_t itemId : itemShopComponent.GetData().saleItems)
        {
            const ItemData* itemData = itemDataProvider.Find(itemId);
            if (!itemData)
            {
                assert(false);

                continue;
            }

            const int32_t width = itemData->GetWidth();
            const int32_t height = itemData->GetHeight();

            NPCShopItemBucket& bucket = temp[width][height];
            bucket.width = width;
            bucket.height = height;

            bucket.items.emplace_back(itemData);
        }

        auto range = temp | std::views::values | std::views::join | std::views::values | std::views::as_rvalue;
        itemShopComponent.Initialize(std::ranges::to<std::vector>(range));

        const std::vector<NPCShopItemBucket>& buckets = itemShopComponent.GetItemBuckets();
        const auto makeBucketIndexes = [&buckets]() -> std::vector<int64_t>
            {
                std::vector<int64_t> result;
                result.reserve(buckets.size());

                for (int64_t i = 0; i < std::ssize(buckets); ++i)
                {
                    result.push_back(i);
                }

                return result;
            };

        GameEntityIdPublisher& idPublisher = _serviceLocator.Get<GameEntityIdPublisher>();
        std::uniform_real_distribution upperBoundDist(0.7, 0.9);

        const int8_t storageCount = itemShopComponent.GetItemStorageCount();

        for (int8_t i = 0; i < storageCount; ++i)
        {
            const double upperbound = upperBoundDist(_mt19937);
            std::vector<int64_t> bucketIndexes = makeBucketIndexes();

            while (!bucketIndexes.empty() && itemShopComponent.GetItemStorageLoadFactor(i) < upperbound)
            {
                const int64_t bucketIndex = std::uniform_int_distribution<int64_t>(0, std::ssize(bucketIndexes) - 1)(_mt19937);
                assert(bucketIndex >= 0 && bucketIndex < std::ssize(bucketIndexes));

                const NPCShopItemBucket& bucket = buckets[bucketIndexes[bucketIndex]];
                if (bucket.items.empty())
                {
                    assert(false);

                    bucketIndexes.erase(bucketIndexes.begin() + bucketIndex);

                    continue;
                }

                const auto& optPosition = itemShopComponent.FindEmptyPosition(i, bucket.width, bucket.height);
                if (!optPosition.has_value())
                {
                    bucketIndexes.erase(bucketIndexes.begin() + bucketIndex);

                    continue;
                }

                const int64_t itemIndex = std::uniform_int_distribution<int64_t>(0, std::ssize(bucket.items) - 1)(_mt19937);
                assert(itemIndex >= 0 && itemIndex < std::ssize(bucket.items));

                PtrNotNull<const ItemData> itemData = bucket.items[itemIndex];
                const int32_t quantity = itemData->GetMaxOverlapCount() == 1 ? 1
                    : std::uniform_int_distribution(1, itemData->GetMaxOverlapCount())(_mt19937);

                auto item = std::make_shared<GameItem>(idPublisher, *itemData, quantity);

                itemShopComponent.AddItem(std::move(item), *optPosition);
            }
        }
    }

    void NPCShopSystem::OnStageExit(GamePlayer& player)
    {
        const auto iter1 = _targetNPC.find(&player);
        if (iter1 == _targetNPC.end())
        {
            return;
        }

        GameNPC& npc = *iter1->second;

        NPCItemShopComponent& itemShopComponent = npc.GetComponent<NPCItemShopComponent>();
        itemShopComponent.RemoveSyncPlayer(player);
    }

    void NPCShopSystem::OpenItemShop(GamePlayer& player, GameNPC& npc)
    {
        if (!_targetNPC.try_emplace(&player, &npc).second)
        {
            SUNLIGHT_LOG_ERROR(_serviceLocator,
                fmt::format("[{}] player already is looking a npc shop. player: {}",
                    GetName(), player.GetCId()));

            return;
        }

        NPCItemShopComponent& itemShopComponent = npc.GetComponent<NPCItemShopComponent>();
        itemShopComponent.AddSyncPlayer(player);

        player.Send(NPCMessageCreator::CreateShopOpen(npc));
    }

    void NPCShopSystem::OnShopSynchronizeRequest(const ZoneMessage& message)
    {
        GamePlayer& player = message.player;

        const auto iter = _targetNPC.find(&player);
        if (iter == _targetNPC.end())
        {
            SUNLIGHT_LOG_WARN(_serviceLocator,
                fmt::format("[{}] client inaccesable npc shop open request. player: {}",
                    GetName(), player.GetCId()));

            return;
        }

        player.Send(NPCMessageCreator::CreateItemSynchroStart(*iter->second));
    }

    void NPCShopSystem::OnShopSynchronizeStop(const ZoneMessage& message)
    {
        GamePlayer& player = message.player;

        const auto iter = _targetNPC.find(&player);
        if (iter == _targetNPC.end())
        {
            return;
        }

        NPCItemShopComponent& itemShopComponent = iter->second->GetComponent<NPCItemShopComponent>();
        itemShopComponent.RemoveSyncPlayer(player);

        _targetNPC.erase(iter);
    }

    void NPCShopSystem::OnPlayerBuyShopItem(const ZoneMessage& message)
    {
        GamePlayer& player = message.player;
        SlPacketReader& reader = message.reader;

        // ZoneMessage.targetId that is sent from client is target npc id
        // but use server-side data for protecting against cheat
        const auto iter = _targetNPC.find(&player);
        if (iter == _targetNPC.end())
        {
            SUNLIGHT_LOG_WARN(_serviceLocator,
                fmt::format("[{}] client inaccesable npc item shop buy request. player: {}",
                    GetName(), player.GetCId()));

            return;
        }

        const int32_t page = reader.Read<int32_t>();
        const int32_t x = reader.Read<int32_t>();
        const int32_t y = reader.Read<int32_t>();

        [[maybe_unused]] const int32_t unk = reader.Read<int32_t>();
        const int32_t quantity = reader.Read<int32_t>();

        BufferReader itemObjectReader = reader.ReadObject();

        itemObjectReader.Skip(8);
        const int32_t itemId = itemObjectReader.Read<int32_t>();

        itemObjectReader.Skip(148);

        const game_entity_id_type targetItemId(itemObjectReader.Read<int32_t>());
        const GameEntityType type = static_cast<GameEntityType>(itemObjectReader.Read<int32_t>());

        if (type != GameEntityType::Item)
        {
            SUNLIGHT_LOG_DEBUG(_serviceLocator,
                fmt::format("[{}] unexpecetd item type. player: {}, target_id: {}, target_type: {}, pos: [{}, {}, {}]",
                    GetName(), player.GetCId(), targetItemId, static_cast<int32_t>(type), page, x, y));
        }

        Get<ItemArchiveSystem>().Purchase(player, *iter->second, targetItemId, itemId, page, x, y, quantity);
    }

    void NPCShopSystem::OnPlayerSellOwnItem(const ZoneMessage& message)
    {
        GamePlayer& player = message.player;
        SlPacketReader& reader = message.reader;

        const auto iter = _targetNPC.find(&player);
        if (iter == _targetNPC.end())
        {
            SUNLIGHT_LOG_WARN(_serviceLocator,
                fmt::format("[{}] client inaccesable npc item shop buy request. player: {}",
                    GetName(), player.GetCId()));

            return;
        }

        [[maybe_unused]]
        const auto [targetItemId, targetItemType] = reader.ReadInt64();

        Get<ItemArchiveSystem>().SellOwnItem(player, *iter->second, game_entity_id_type(targetItemId));
    }
}
