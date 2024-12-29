#include "npc_shop_system.h"

#include <boost/unordered/unordered_flat_set.hpp>

#include "sl/generator/game/component/npc_item_shop_component.h"
#include "sl/generator/game/component/player_npc_shop_component.h"
#include "sl/generator/game/entity/game_item.h"
#include "sl/generator/game/entity/game_npc.h"
#include "sl/generator/game/entity/game_player.h"
#include "sl/generator/game/message/zone_message.h"
#include "sl/generator/game/message/creator/npc_message_creator.h"
#include "sl/generator/game/system/item_archive_system.h"
#include "sl/generator/game/system/scene_object_system.h"
#include "sl/generator/game/time/game_time_service.h"
#include "sl/generator/game/zone/stage.h"
#include "sl/generator/game/zone/service/game_entity_id_publisher.h"
#include "sl/generator/game/zone/service/zone_execution_service.h"
#include "sl/generator/service/gamedata/gamedata_provide_service.h"
#include "sl/generator/service/gamedata/item/item_data_provider.h"
#include "sl/generator/service/gamedata/shop/item_shop_data.h"
#include "sl/generator/service/game_cheat_log/game_cheat_log_service.h"

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
        Add(stage.Get<SceneObjectSystem>());
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

        std::unordered_map<int32_t, std::unordered_map<int32_t, NPCShopItemBucket>> temp;

        NPCItemShopComponent& itemShopComponent = npc.GetComponent<NPCItemShopComponent>();

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

        CreateShopItems(npc);
    }

    bool NPCShopSystem::Roll(GameNPC& npc)
    {
        NPCItemShopComponent* itemShopComponent = npc.FindComponent<NPCItemShopComponent>();
        if (!itemShopComponent)
        {
            return false;
        }

        if (itemShopComponent->GetSynchronizePlayerCount() > 0)
        {
            for (const GameItem& item : itemShopComponent->GetItemRange())
            {
                itemShopComponent->Synchronize(NPCMessageCreator::CreateNPCItemRemove(npc, item.GetId(), item.GetType()));
            }
        }

        itemShopComponent->ClearItems();
        
        CreateShopItems(npc);

        return true;
    }

    void NPCShopSystem::OnStageExit(GamePlayer& player)
    {
        const PlayerNPCShopComponent& playerNPCShopComponent = player.GetNPCShopComponent();

        if (!playerNPCShopComponent.HasShoppingNPC())
        {
            return;
        }

        GameNPC& npc = playerNPCShopComponent.GetShoppingNPC();

        NPCItemShopComponent& itemShopComponent = npc.GetComponent<NPCItemShopComponent>();
        itemShopComponent.RemoveSyncPlayer(player);
    }

    void NPCShopSystem::OpenItemShop(GamePlayer& player, GameNPC& npc)
    {
        PlayerNPCShopComponent& playerNPCShopComponent = player.GetNPCShopComponent();

        if (playerNPCShopComponent.HasShoppingNPC())
        {
            _serviceLocator.Get<GameCheatLogService>().Log(GameCheatLogType::NPCShop,
                player.GetName(), fmt::format("player is already open a npc shop. old: {}, new: {}",
                    playerNPCShopComponent.GetShoppingNPC().GetId(), npc.GetId()));

            return;
        }

        playerNPCShopComponent.SetShoppingNPC(&npc);

        player.Send(NPCMessageCreator::CreateShopOpen(npc));

        if (NPCItemShopComponent* itemShopComponent = npc.FindComponent<NPCItemShopComponent>(); itemShopComponent)
        {
            itemShopComponent->AddSyncPlayer(player);

            if (!itemShopComponent->IsVisitedAfterRoll())
            {
                itemShopComponent->SetVisitedAfterRoll(true);

                ConfigureNPCShopRollTimer(npc);
            }
        }
    }

    void NPCShopSystem::OnShopSynchronizeRequest(const ZoneMessage& message)
    {
        GamePlayer& player = message.player;

        PlayerNPCShopComponent& playerNPCShopComponent = player.GetNPCShopComponent();

        if (!playerNPCShopComponent.HasShoppingNPC())
        {
            _serviceLocator.Get<GameCheatLogService>().Log(GameCheatLogType::NPCShop,
                player.GetName(), fmt::format("invalid client npc shop open request"));

            return;
        }

        player.Send(NPCMessageCreator::CreateItemSynchroStart(playerNPCShopComponent.GetShoppingNPC()));
    }

    void NPCShopSystem::OnShopSynchronizeStop(const ZoneMessage& message)
    {
        GamePlayer& player = message.player;

        PlayerNPCShopComponent& playerNPCShopComponent = player.GetNPCShopComponent();

        if (!playerNPCShopComponent.HasShoppingNPC())
        {
            return;
        }

        GameNPC& npc = playerNPCShopComponent.GetShoppingNPC();
        playerNPCShopComponent.SetShoppingNPC(nullptr);

        if (NPCItemShopComponent* itemShopComponent = npc.FindComponent<NPCItemShopComponent>(); itemShopComponent)
        {
            itemShopComponent->RemoveSyncPlayer(player);
        }
    }

    void NPCShopSystem::OnPlayerBuyShopItem(const ZoneMessage& message)
    {
        GamePlayer& player = message.player;
        SlPacketReader& reader = message.reader;

        PlayerNPCShopComponent& playerNPCShopComponent = player.GetNPCShopComponent();

        if (!playerNPCShopComponent.HasShoppingNPC())
        {
            _serviceLocator.Get<GameCheatLogService>().Log(GameCheatLogType::NPCShop,
                player.GetName(), fmt::format("invalid client npc item shop buy request"));

            return;
        }

        const int32_t page = reader.Read<int32_t>();
        const int32_t x = reader.Read<int32_t>();
        const int32_t y = reader.Read<int32_t>();

        const int32_t unk = reader.Read<int32_t>();
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

        Get<ItemArchiveSystem>().Purchase(player, playerNPCShopComponent.GetShoppingNPC(), targetItemId, itemId, page, x, y, quantity);

        if (unk == 1)
        {
            SUNLIGHT_LOG_DEBUG(_serviceLocator,
                fmt::format("[{}] unk value 1 found. player: {}, npc: {}, item_id, quantity: {}",
                    GetName(), player.GetCId(), playerNPCShopComponent.GetShoppingNPC().GetId(), itemId, quantity));
        }
    }

    void NPCShopSystem::OnPlayerSellOwnItem(const ZoneMessage& message)
    {
        GamePlayer& player = message.player;
        SlPacketReader& reader = message.reader;

        PlayerNPCShopComponent& playerNPCShopComponent = player.GetNPCShopComponent();

        if (!playerNPCShopComponent.HasShoppingNPC())
        {
            _serviceLocator.Get<GameCheatLogService>().Log(GameCheatLogType::NPCShop,
                player.GetName(), fmt::format("invalid client npc item shop sell request"));

            return;
        }

        [[maybe_unused]]
        const auto [targetItemId, targetItemType] = reader.ReadInt64();

        Get<ItemArchiveSystem>().SellOwnItem(player, playerNPCShopComponent.GetShoppingNPC(), game_entity_id_type(targetItemId));
    }

    void NPCShopSystem::OnBarberPayment(const ZoneMessage& message)
    {
        GamePlayer& player = message.player;
        SlPacketReader& reader = message.reader;

        const int32_t cost = reader.Read<int32_t>();
        if (cost > 0)
        {
            _serviceLocator.Get<GameCheatLogService>().Log(GameCheatLogType::NPCShop,
                player.GetName(), fmt::format("money printing cheat detected. gold: {}", cost));
        }
        else if (cost == 0)
        {
            return;
        }

        if (!Get<ItemArchiveSystem>().Charge(player, -cost))
        {
            _serviceLocator.Get<GameCheatLogService>().Log(GameCheatLogType::NPCShop,
                player.GetName(), fmt::format("fail to charge barber payment. value: {}", cost));
        }
    }

    void NPCShopSystem::CreateShopItems(GameNPC& npc)
    {
        NPCItemShopComponent& itemShopComponent = npc.GetComponent<NPCItemShopComponent>();

        const bool shouldSynchronize = itemShopComponent.GetItemStorageCount() > 0;

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
                auto itemPtr = item.get();

                itemShopComponent.AddItem(std::move(item), *optPosition);

                if (shouldSynchronize)
                {
                    itemShopComponent.Synchronize(NPCMessageCreator::CreateNPCItemAdd(npc, *itemPtr));
                }
            }
        }
    }

    void NPCShopSystem::ConfigureNPCShopRollTimer(GameNPC& npc)
    {
        NPCItemShopComponent& itemShopComponent = npc.GetComponent<NPCItemShopComponent>();
        assert(!itemShopComponent.HasRollTimer());

        std::uniform_int_distribution rollTimeDist(45, 90);
        const std::chrono::minutes nextRollTime(rollTimeDist(_mt19937));

        itemShopComponent.SetRollTimer(true);
        itemShopComponent.SetNextRollTimePoint(GameTimeService::Now() + nextRollTime);

        _serviceLocator.Get<ZoneExecutionService>().AddTimer(nextRollTime,
            [this, npcId = npc.GetId()]()
            {
                this->OnRollTimerEnd(npcId);
            });
    }

    void NPCShopSystem::OnRollTimerEnd(game_entity_id_type npcId)
    {
        GameEntity* npc = Get<SceneObjectSystem>().FindEntity(GameEntityType::NPC, npcId);
        if (!npc)
        {
            return;
        }

        NPCItemShopComponent& itemShopComponent = npc->GetComponent<NPCItemShopComponent>();
        assert(itemShopComponent.HasRollTimer());

        if (itemShopComponent.HasSyncPlayer())
        {
            constexpr auto delay = std::chrono::minutes(1);
            itemShopComponent.SetNextRollTimePoint(itemShopComponent.GetNextRollTimePoint() + delay);

            _serviceLocator.Get<ZoneExecutionService>().AddTimer(delay,
                [this, npcId = npc->GetId()]()
                {
                    this->OnRollTimerEnd(npcId);
                });

            return;
        }

        itemShopComponent.SetVisitedAfterRoll(false);
        itemShopComponent.SetRollTimer(false);
        itemShopComponent.SetNextRollTimePoint(game_time_point_type::max());

        Roll(*npc->Cast<GameNPC>());
    }
}
