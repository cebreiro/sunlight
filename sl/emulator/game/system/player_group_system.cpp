#include "player_group_system.h"

#include <boost/container/small_vector.hpp>

#include "sl/emulator/game/component/player_group_component.h"
#include "sl/emulator/game/component/player_item_component.h"
#include "sl/emulator/game/component/player_skill_component.h"
#include "sl/emulator/game/component/scene_object_component.h"
#include "sl/emulator/game/component/street_vendor_host_component.h"
#include "sl/emulator/game/contants/group/game_group.h"
#include "sl/emulator/game/entity/game_item.h"
#include "sl/emulator/game/entity/game_player.h"
#include "sl/emulator/game/entity/game_stored_item.h"
#include "sl/emulator/game/message/zone_community_message.h"
#include "sl/emulator/game/message/zone_message.h"
#include "sl/emulator/game/message/creator/game_player_message_creator.h"
#include "sl/emulator/game/message/creator/item_archive_message_creator.h"
#include "sl/emulator/game/message/creator/item_mix_message_creator.h"
#include "sl/emulator/game/message/creator/item_trade_message_creator.h"
#include "sl/emulator/game/message/creator/street_vendor_message_creator.h"
#include "sl/emulator/game/system/entity_view_range_system.h"
#include "sl/emulator/game/system/item_archive_system.h"
#include "sl/emulator/game/system/item_trade_system.h"
#include "sl/emulator/game/system/scene_object_system.h"
#include "sl/emulator/game/zone/stage.h"
#include "sl/emulator/game/zone/service/game_entity_id_publisher.h"
#include "sl/emulator/server/client/game_client.h"
#include "sl/emulator/service/gamedata/gamedata_provide_service.h"
#include "sl/emulator/service/gamedata/item/item_data.h"
#include "sl/emulator/service/gamedata/item_mix/item_mix_data_provider.h"

namespace sunlight
{
    PlayerGroupSystem::PlayerGroupSystem(const ServiceLocator& serviceLocator)
        : _serviceLocator(serviceLocator)
        , _mt(std::random_device{}())
    {
    }

    PlayerGroupSystem::~PlayerGroupSystem()
    {
    }

    void PlayerGroupSystem::InitializeSubSystem(Stage& stage)
    {
        Add(stage.Get<EntityViewRangeSystem>());
        Add(stage.Get<ItemTradeSystem>());
        Add(stage.Get<ItemArchiveSystem>());
        Add(stage.Get<SceneObjectSystem>());
    }

    bool PlayerGroupSystem::Subscribe(Stage& stage)
    {
        if (!stage.AddSubscriber(ZoneMessageType::STATE_PROPOSITION,
            std::bind_front(&PlayerGroupSystem::HandleStateProposition, this)))
        {
            return false;
        }

        if (!stage.AddSubscriber(ZoneMessageType::REQUEST_CREATE_GROUP,
            std::bind_front(&PlayerGroupSystem::HandleCreateGroup, this)))
        {
            return false;
        }

        if (!stage.AddSubscriber(ZoneMessageType::GROUP_MSG,
            std::bind_front(&PlayerGroupSystem::HandleGroupMessage, this)))
        {
            return false;
        }

        return true;
    }

    auto PlayerGroupSystem::GetName() const -> std::string_view
    {
        return "player_group_system";
    }

    auto PlayerGroupSystem::GetClassId() const -> game_system_id_type
    {
        return GameSystem::GetClassId<PlayerGroupSystem>();
    }

    void PlayerGroupSystem::OnStageExit(GamePlayer& player)
    {
        const PlayerGroupComponent& groupComponent = player.GetGroupComponent();
        if (!groupComponent.HasGroup())
        {
            return;
        }

        const auto iter = _gameGroups.find(groupComponent.GetGroupId());
        assert(iter != _gameGroups.end());

        if (iter->second->IsHost(player))
        {
            OnHostExit(*iter->second, player);
        }
        else
        {
            OnGuestExit(*iter->second, player);
        }
    }

    void PlayerGroupSystem::AddStreetVendorGuest(int32_t groupId, GamePlayer& player)
    {
        PlayerGroupComponent& playerGroupComponent = player.GetGroupComponent();
        if (playerGroupComponent.HasGroup())
        {
            return;
        }

        GameGroup* group = FindGroup(groupId);
        if (!group || group->GetType() != GameGroupType::StreetVendor)
        {
            return;
        }

        if (group->IsHost(player) || group->IsGuest(player))
        {
            return;
        }

        if (!group->GetHost().GetComponent<StreetVendorHostComponent>().IsOpen())
        {
            return;
        }

        group->AddGuest(player);

        playerGroupComponent.SetGroupId(group->GetId());
        playerGroupComponent.SetGroupType(group->GetType());

        player.Defer(StreetVendorMessageCreator::CreateGroupCreate(group->GetId()));
        player.Defer(ItemTradeMessageCreator::CreateGroupHostData(group->GetId(), group->GetHost()));
        player.FlushDeferred();

        return;
    }

    void PlayerGroupSystem::HandleStateProposition(const ZoneMessage& message)
    {
        // client's group state broadcast request

        GamePlayer& player = message.player;
        SlPacketReader& reader = message.reader;

        GameGroupState newState;
        newState.title = reader.ReadString();
        newState.groupId = reader.Read<int32_t>();
        newState.groupType = static_cast<GameGroupType>(reader.Read<int32_t>());
        newState.type = reader.Read<int32_t>();

        player.GetGroupComponent().SetGroupState(newState);

        constexpr bool includeSelf = true;

        Get<EntityViewRangeSystem>().Broadcast(player,
            GamePlayerMessageCreator::CreatePlayerStateProposition(player, newState), includeSelf);
    }

    void PlayerGroupSystem::HandleCreateGroup(const ZoneCommunityMessage& message)
    {
        GamePlayer& player = message.player;
        SlPacketReader& reader = message.reader;

        BufferReader objectReader = reader.ReadObject();

        std::string title = objectReader.ReadString(64);

        [[maybe_unused]] const int32_t unk1 = objectReader.Read<int32_t>();
        const GameGroupType groupType = static_cast<GameGroupType>(objectReader.Read<int32_t>());

        std::array<char, 64> unk2 = {};
        objectReader.ReadBuffer(unk2.data(), std::ssize(unk2));

        [[maybe_unused]] const int32_t hostId = objectReader.Read<int32_t>();
        [[maybe_unused]] const int32_t hostType = objectReader.Read<int32_t>(); // check
        [[maybe_unused]] const int32_t unk3 = objectReader.Read<int32_t>();
        [[maybe_unused]] const int32_t unk4 = objectReader.Read<int32_t>();
        [[maybe_unused]] const int32_t unk5 = objectReader.Read<int32_t>();

        PlayerGroupComponent& groupComponent = player.GetGroupComponent();
        if (groupComponent.HasGroup())
        {
            // TODO: impl

            return;
        }

        switch (groupType)
        {
        case GameGroupType::Trade:
        {
            const int32_t groupId = ++_nextGroupId;
            _gameGroups[groupId] = std::make_unique<GameGroup>(groupId, groupType, player);

            groupComponent.SetGroupId(groupId);
            groupComponent.SetGroupType(groupType);

            player.Send(ItemTradeMessageCreator::CreateGroupCreate(groupId));

            Get<ItemTradeSystem>().Start(player);
        }
        break;
        case GameGroupType::StreetVendor:
        {
            const int32_t groupId = ++_nextGroupId;
            _gameGroups[groupId] = std::make_unique<GameGroup>(groupId, groupType, player);

            groupComponent.SetGroupId(groupId);
            groupComponent.SetGroupType(groupType);

            assert(!player.HasComponent<StreetVendorHostComponent>());
            player.AddComponent(std::make_unique<StreetVendorHostComponent>());

            player.Defer(StreetVendorMessageCreator::CreateGroupCreate(groupId));
            player.Defer(StreetVendorMessageCreator::CreatePageItemDisplay(groupId, 0, player));
            player.FlushDeferred();
        }
        break;
        case GameGroupType::ItemMix:
        {
            const int32_t groupId = ++_nextGroupId;
            _gameGroups[groupId] = std::make_unique<GameGroup>(groupId, groupType, player);

            groupComponent.SetGroupId(groupId);
            groupComponent.SetGroupType(groupType);

            player.Defer(ItemMixMessageCreator::CreateGroupCreate(groupId));
            player.FlushDeferred();
        }
        break;
        case GameGroupType::Unk04:
        case GameGroupType::Unk05:
        case GameGroupType::Interior:
        default:
            SUNLIGHT_LOG_WARN(_serviceLocator,
                fmt::format("[{}] unhandled player group. player: {}, group: {}, buffer: {}",
                    GetName(), player.GetCId(), ToString(groupType), reader.GetBuffer().ToString()));
        }
    }

    void PlayerGroupSystem::HandleGroupMessage(const ZoneCommunityMessage& message)
    {
        GamePlayer& player = message.player;
        SlPacketReader& reader = message.reader;

        const int32_t groupId = reader.Read<int32_t>();
        const int32_t messageType = reader.Read<int32_t>();

        const auto iter = _gameGroups.find(groupId);
        if (iter == _gameGroups.end())
        {
            return;
        }

        GameGroup& group = *iter->second;

        bool handled = false;

        switch (messageType)
        {
        case 100: // join req
        {
            switch (group.GetType())
            {
            case GameGroupType::Trade:
            {
                if (group.HasGuest())
                {
                    player.Send(ItemTradeMessageCreator::CreateGroupJoinFail(groupId));
                }
                else
                {
                    PlayerGroupComponent& groupComponent = player.GetGroupComponent();

                    if (group.IsHost(player) || groupComponent.HasGroup())
                    {
                        player.Send(ItemTradeMessageCreator::CreateGroupJoinFail(groupId));

                        return;
                    }

                    group.AddGuest(player);
                    groupComponent.SetGroupId(group.GetId());
                    groupComponent.SetGroupType(GameGroupType::Trade);


                    GamePlayer& host = group.GetHost();
                    host.Send(ItemTradeMessageCreator::CreateGroupGuestData(group.GetId(), player));

                    player.Defer(ItemTradeMessageCreator::CreateGroupHostData(group.GetId(), host));
                    player.Defer(ItemTradeMessageCreator::CreateGroupHostItemData(group.GetId(), host));
                    player.FlushDeferred();

                    Get<ItemTradeSystem>().Start(player);
                }

                handled = true;
            }
            break;
            case GameGroupType::StreetVendor:
            {
                if (group.IsHost(player))
                {
                    assert(false);

                    handled = true;
                    break;
                }

                if (group.IsGuest(player))
                {
                    handled = true;
                    break;
                }

                PlayerGroupComponent& playerGroupComponent = player.GetGroupComponent();
                if (playerGroupComponent.HasGroup())
                {
                    handled = true;
                    break;
                }

                group.AddGuest(player);

                playerGroupComponent.SetGroupId(group.GetId());
                playerGroupComponent.SetGroupType(group.GetType());

                player.Send(ItemTradeMessageCreator::CreateGroupHostData(group.GetId(), group.GetHost()));

                handled = true;
            }
            break;
            case GameGroupType::Null:
            case GameGroupType::Unk04:
            case GameGroupType::Unk05:
            case GameGroupType::ItemMix:
            case GameGroupType::Interior:
            default:;
            }
        }
        break;
        case 900:
        {
            switch (group.GetType())
            {
            case GameGroupType::Trade:
            {
                handled = HandleTradeMessage(group, player, reader);
            }
            break;
            case GameGroupType::StreetVendor:
            {
                handled = HandleStreetVendorMessage(group, player, reader);
            }
            break;
            case GameGroupType::ItemMix:
            {
                handled = HandleItemMixMessage(group, player, reader);
            }
            break;
            case GameGroupType::Null:
            case GameGroupType::Unk04:
            case GameGroupType::Unk05:
            case GameGroupType::Interior:
            default:;
            }
        }
        break;
        default:;
        }

        if (!handled)
        {
            SUNLIGHT_LOG_WARN(_serviceLocator,
                fmt::format("[{}] unhandled group message. player: {}, group: {}, message_type: {}, buffer: {}",
                    GetName(), player.GetCId(), groupId, messageType, reader.GetBuffer().ToString()));
        }
    }

    void PlayerGroupSystem::OnHostExit(GameGroup& group, GamePlayer& host)
    {
        assert(group.IsHost(host));

        if (group.GetType() == GameGroupType::Trade)
        {
            for (GamePlayer& guest : group.GetGuests() | notnull::reference)
            {
                ProcessTradeFail(group, guest);
            }

            ProcessTradeFail(group, host);
        }
        else if (group.GetType() == GameGroupType::StreetVendor)
        {
            host.GetGroupComponent().Clear();

            StreetVendorHostComponent& streetVendorHostComponent = host.GetComponent<StreetVendorHostComponent>();

            for (const game_entity_id_type storedItemId : streetVendorHostComponent.GetStoredItems())
            {
                [[maybe_unused]]
                const bool removed = Get<SceneObjectSystem>().RemoveStoredItem(storedItemId);
                assert(removed);
            }

            for (GamePlayer& guest : group.GetGuests() | notnull::reference)
            {
                guest.Send(StreetVendorMessageCreator::CreateGroupShutdown(group.GetId()));

                guest.GetGroupComponent().Clear();
            }

            host.RemoveComponent<StreetVendorHostComponent>();
        }
        else if (group.GetType() == GameGroupType::ItemMix)
        {
            host.GetGroupComponent().Clear();
        }
        else
        {
            assert(false);
        }

        _gameGroups.erase(group.GetId());
    }

    void PlayerGroupSystem::OnGuestExit(GameGroup& group, GamePlayer& guest)
    {
        assert(group.IsGuest(guest));

        if (group.GetType() == GameGroupType::Trade)
        {
            OnHostExit(group, group.GetHost());

            return;
        }
        else if (group.GetType() == GameGroupType::StreetVendor)
        {
            guest.GetGroupComponent().Clear();
            group.RemoveGuest(guest);

            guest.Send(StreetVendorMessageCreator::CreateGroupShutdown(group.GetId()));
        }
        else
        {
            assert(false);
        }
    }

    bool PlayerGroupSystem::HandleTradeMessage(GameGroup& group, GamePlayer& player, SlPacketReader& reader)
    {
        const int32_t tradeMessage = reader.Read<int32_t>();
        switch (tradeMessage)
        {
        case 1601:
        {
            if (group.IsHost(player))
            {
                OnHostExit(group, player);
            }
            else
            {
                OnGuestExit(group, player);
            }

            return true;
        }
        case 1001: // lift item
        {
            const auto [targetItemId, targetItemType] = reader.ReadInt64();

            if (!Get<ItemTradeSystem>().LiftItem(group, player, game_entity_id_type(targetItemId)))
            {
                player.GetClient().Disconnect();
            }
            return true;
        }
        case 1002: // lower item
        {
            const int32_t x = reader.Read<int32_t>();
            const int32_t y = reader.Read<int32_t>();
            const auto [targetItemId, targetItemType] = reader.ReadInt64();

            if (!Get<ItemTradeSystem>().LowerItem(group, player, x, y, game_entity_id_type(targetItemId)))
            {
                player.GetClient().Disconnect();
            }

            return true;
        }
        case 1003:
        {
            const int32_t gold = reader.Read<int32_t>();

            if (!Get<ItemTradeSystem>().ChangeGold(group, player, gold))
            {
                player.GetClient().Disconnect();
            }

            return true;
        }
        case 1005:
        {
            group.Broadcast(ItemTradeMessageCreator::CreateTradeConfirm(group.GetId()), player.GetId());

            group.AddTradeConfirmPlayer(player);

            if (group.GetTradeConfirmPlayerCount() >= 2)
            {
                GamePlayer& host = group.GetHost();
                GamePlayer& guest = *group.GetGuests()[0];

                if (Get<ItemTradeSystem>().Commit(host, guest))
                {
                    group.Broadcast(ItemTradeMessageCreator::CreateTradeSuccess(group.GetId()), std::nullopt);
                }
                else
                {
                    ProcessTradeFail(group, host);
                    ProcessTradeFail(group, guest);
                }

                host.GetGroupComponent().Clear();
                guest.GetGroupComponent().Clear();

                _gameGroups.erase(group.GetId());
            }

            return true;
        }
        }

        return false;
    }

    void PlayerGroupSystem::ProcessTradeFail(GameGroup& group, GamePlayer& player)
    {
        if (Get<ItemTradeSystem>().Rollback(player))
        {
            player.Send(ItemTradeMessageCreator::CreateTradeFail(group.GetId()));
        }
        else
        {
            player.GetClient().Disconnect();
        }

        player.GetGroupComponent().Clear();
    }

    bool PlayerGroupSystem::HandleStreetVendorMessage(GameGroup& group, GamePlayer& player, SlPacketReader& reader)
    {
        const int32_t message = reader.Read<int32_t>();
        switch (message)
        {
        case 1601:
        {
            if (group.IsHost(player))
            {
                OnHostExit(group, player);
            }
            else
            {
                OnGuestExit(group, player);
            }

            return true;
        }
        case 1302:
        {
            const int32_t index = reader.Read<int32_t>();

            if (player.GetComponent<StreetVendorHostComponent>().IsOpen())
            {
                player.Send(StreetVendorMessageCreator::CreatePageItemDisplay(group.GetId(), index / 2 * 2, player));

                return true;
            }

            if (!Get<ItemArchiveSystem>().OnVendorSaleStorageClick(player, index))
            {
                return true;
            }

            const int32_t page = (index / 2) * 2;

            player.Send(StreetVendorMessageCreator::CreatePageItemDisplay(group.GetId(), page, player));

            return true;
        }
        case 1304: // change current page
        {
            const int32_t page = reader.Read<int32_t>();

            if (group.IsHost(player))
            {
                player.Send(StreetVendorMessageCreator::CreatePageItemDisplay(group.GetId(), (page / 2) * 2, player));
            }
            else
            {
                player.Send(StreetVendorMessageCreator::CreatePageItemDisplay(group.GetId(), (page / 2) * 2, group.GetHost()));
            }

            return true;
        }
        case 1305:
        {
            const int32_t index = reader.Read<int32_t>();
            const int32_t price = reader.Read<int32_t>();

            StreetVendorHostComponent& hostComponent = player.GetComponent<StreetVendorHostComponent>();

            if (hostComponent.IsOpen())
            {
                player.Send(StreetVendorMessageCreator::CreatePageItemDisplay(group.GetId(), index / 2 * 2, player));

                return true;
            }

            if (!hostComponent.IsValid(index))
            {
                SUNLIGHT_LOG_ERROR(_serviceLocator,
                    fmt::format("[{}] fail to configure item price. invalid index. player: {}, group: {}, index: {}, price: {}",
                        GetName(), player.GetCId(), group.GetId(), index, price));

                return true;
            }

            hostComponent.SetItemPrice(index, price);

            player.Send(StreetVendorMessageCreator::CreateItemPriceChange(group.GetId(), index, price));

            return true;
        }
        case 1306:
        {
            const PlayerItemComponent& itemComponent = player.GetItemComponent();
            StreetVendorHostComponent& hostComponent = player.GetComponent<StreetVendorHostComponent>();

            std::array<std::tuple<const GameItem*, int32_t, int32_t>, GameConstant::MAX_STREET_VENDOR_STORED_ITEM_SIZE> storedItemData = {};
            int32_t k = 0;

            int32_t nullItemCount = 0;

            for (int32_t i = 0; i < GameConstant::MAX_STREET_VENDOR_PAGE_SIZE; ++i)
            {
                if (const GameItem* saleItem = itemComponent.GetVendorSaleItem(i); saleItem)
                {
                    const int32_t price = hostComponent.GetItemPrice(i);
                    if (price <= 0)
                    {
                        player.Send(StreetVendorMessageCreator::CreateOpenResult(group.GetId(), StreetVendorStartResult::NoSaleItemPrice));

                        return true;
                    }

                    if (k < std::ssize(storedItemData))
                    {
                        storedItemData[k++] = std::tuple{ saleItem, i, price };
                    }
                }
                else
                {
                    ++nullItemCount;
                }
            }

            if (nullItemCount >= GameConstant::MAX_STREET_VENDOR_PAGE_SIZE)
            {
                player.Send(StreetVendorMessageCreator::CreateOpenResult(group.GetId(), StreetVendorStartResult::NoSaleItem));

                return true;
            }

            for (int32_t i = 0; i < GameConstant::MAX_STREET_VENDOR_STORED_ITEM_SIZE; ++i)
            {
                const auto [saleItem, page, price] = storedItemData[i];
                if (saleItem)
                {
                    SpawnStoredItem(group, player, *saleItem, page, price, i);
                }
                else
                {
                    break;
                }
            }

            hostComponent.SetOpen(true);

            player.Send(StreetVendorMessageCreator::CreateOpenResult(group.GetId(), StreetVendorStartResult::Success));

            return true;
        }
        case 1308: // purchase
        {
            const int32_t page = reader.Read<int32_t>();

            StreetVendorPurchaseResult result = StreetVendorPurchaseResult::AlreadySold;
            bool soldOut = false;

            do
            {
                if (group.IsHost(player))
                {
                    break;
                }

                GamePlayer& host = group.GetHost();
                StreetVendorHostComponent& hostComponent = host.GetComponent<StreetVendorHostComponent>();

                if (!hostComponent.IsValid(page))
                {
                    break;
                }

                const int32_t price = hostComponent.GetItemPrice(page);

                PlayerItemComponent& playerItemComponent = player.GetItemComponent();
                if (playerItemComponent.GetGold() < price)
                {
                    result = StreetVendorPurchaseResult::LockOfMoney;

                    break;
                }

                PlayerItemComponent& hostItemComponent = host.GetItemComponent();

                const GameItem* targetItem = hostItemComponent.GetVendorSaleItem(page);
                if (!targetItem)
                {
                    break;
                }

                if (!Get<ItemArchiveSystem>().PurchaseStreetVendorItem(host, player, targetItem->GetId(), price))
                {
                    result = StreetVendorPurchaseResult::LockOfSpace;

                    break;
                }

                result = StreetVendorPurchaseResult::Success;

                soldOut = hostItemComponent.GetVendorSaleItemSize() <= 0;

                if (const std::optional<game_entity_id_type> storedItemId = hostComponent.GetStoredItem(page);
                    storedItemId.has_value())
                {
                    // accessed by clicking field displayed 'stored item'

                    const auto& storedItem = Get<SceneObjectSystem>().FindEntity(GameStoredItem::TYPE, *storedItemId);
                    if (!storedItem)
                    {
                        assert(false);

                        break;
                    }

                    const int32_t offset = storedItem->Cast<GameStoredItem>()->GetFieldOffset();

                    for (const auto& [saleItemPage, saleItem] : hostItemComponent.GetVendorSaleItems())
                    {
                        if (hostComponent.IsDisplayedItemPage(saleItemPage))
                        {
                            continue;
                        }

                        SpawnStoredItem(group, host, saleItem, saleItemPage, hostComponent.GetItemPrice(saleItemPage), offset);

                        break;
                    }


                    Get<SceneObjectSystem>().RemoveStoredItem(*storedItemId);
                    hostComponent.RemoveStoredItem(page);
                }

                player.Send(StreetVendorMessageCreator::CreatePageItemDisplay(group.GetId(), page / 2 * 2, group.GetHost()));

            } while (false);

            player.Send(StreetVendorMessageCreator::CreateItemPurchaseResult(group.GetId(), result));

            if (soldOut)
            {
                group.GetHost().Send(StreetVendorMessageCreator::CreateVendorItemSoldOut(group.GetId()));

                OnHostExit(group, group.GetHost());
            }

            return true;
        }
        }

        return false;
    }

    bool PlayerGroupSystem::HandleItemMixMessage(GameGroup& group, GamePlayer& player, SlPacketReader& reader)
    {
        const int32_t message = reader.Read<int32_t>();
        switch (message)
        {
        case 1601: // suspend ?
        case 1106: // exit ?
        {
            OnHostExit(group, player);

            return true;
        }
        case 1108: // click target item to rollback
        {
            Get<ItemArchiveSystem>().TryRollbackMixItem(player);

            player.Send(ItemTradeMessageCreator::CreateGoldChangeResult(group.GetId()));
        }
        break;
        case 1150: // lift item
        {
            [[maybe_unused]]
            const int32_t unk1 = reader.Read<int32_t>();
            const auto [itemId, itemType] = reader.ReadInt64();

            if (!Get<ItemArchiveSystem>().OnMixItemLift(player, game_entity_id_type(itemId)))
            {
                SUNLIGHT_LOG_ERROR(_serviceLocator,
                    fmt::format("[{}] fail to lift mix item. player: {}, item_id: {}",
                        GetName(), player.GetCId(), itemId));
            }
        }
        break;
        case 1151: // lower item to item mix window
        {
            [[maybe_unused]]
            const int32_t unk1 = reader.Read<int32_t>();
            const auto [itemId, itemType] = reader.ReadInt64();

            if (Get<ItemArchiveSystem>().OnMixItemLower(player, game_entity_id_type(itemId)))
            {
                player.Send(ItemMixMessageCreator::CreateItemLowerResult(group.GetId()));
            }
            else
            {
                SUNLIGHT_LOG_ERROR(_serviceLocator,
                    fmt::format("[{}] fail to lower mix item. player: {}, item_id: {}",
                        GetName(), player.GetCId(), itemId));
            }
        }
        break;
        case 1105: // request mix
        {
            const int32_t itemId = reader.Read<int32_t>();
            const int32_t skillId = reader.Read<int32_t>();

            [[maybe_unused]]
            const auto [mixToolItemId, unused] = reader.ReadInt64();

            bool result = false;
            std::string log;

            do
            {
                PlayerSkillComponent& skillComponent = player.GetSkillComponent();
                const PlayerSkill* skill = skillComponent.FindSkill(skillId);

                if (!skill)
                {
                    log = fmt::format("fail to find ski. player: {}, skill: {}", player.GetCId(), skillId);

                    break;
                }

                const ItemMixDataProvider& itemMixDataProvider = _serviceLocator.Get<GameDataProvideService>().GetItemMixDataProvider();
                const ItemMixGroupMemberData* itemMixData = itemMixDataProvider.Find(skillId)->Find(skill->GetLevel(), itemId);

                if (!itemMixData)
                {
                    log = fmt::format("fail to find item mix data. player: {}, skill {}, skill_level: {}, item_id: {}",
                        player.GetCId(), skillId, skill->GetLevel(), itemId);

                    break;
                }

                int32_t materialLevel = 0;

                const std::vector<ItemMixMaterial>& dataMaterials = itemMixData->GetMaterials();
                boost::container::small_vector<ItemMixMaterial, 4> materials(dataMaterials.begin(), dataMaterials.end());

                constexpr int32_t itemIdGradeRuleConstant = 10000000;

                for (const GameItem& mixItem : player.GetItemComponent().GetMixItems())
                {
                    const int32_t mixItemId = mixItem.GetData().GetId();
                    const auto iter = std::ranges::find_if(materials, [mixItemId](const ItemMixMaterial& material) -> bool
                        {
                            if (material.quantity <= 0)
                            {
                                return false;
                            }

                            return (mixItemId % itemIdGradeRuleConstant) == (material.itemId % itemIdGradeRuleConstant);
                        });
                    if (iter != materials.end())
                    {
                        iter->quantity -= mixItem.GetQuantity();

                        materialLevel += std::max(0, (mixItemId / itemIdGradeRuleConstant) - 1);
                    }
                }

                if (std::ranges::any_of(materials, [](const ItemMixMaterial& material) -> bool
                    {
                        return material.quantity > 0;
                    }))
                {
                    log = fmt::format("lack of mix item material. player: {}, skill {}, skill_level: {}, item_id: {}",
                        player.GetCId(), skillId, skill->GetLevel(), itemId);

                    break;
                }

                const int32_t gradeLevel = skill->GetLevel() + materialLevel;
                const std::array<int32_t, item_mix_grade_weight_size>* weightData = itemMixDataProvider.FindWeight(itemMixData->GetGradeType(), gradeLevel);

                if (!weightData)
                {
                    log = fmt::format("fail to find weight data. player: {}, skill {}, skill_level: {}, item_id: {}, grade_type: {}grade_level: {}",
                        player.GetCId(), skillId, skill->GetLevel(), itemId, itemMixData->GetGradeType(), gradeLevel);

                    break;
                }

                int32_t prev = 0;
                boost::container::small_vector<std::pair<ItemMixGradeWeightType, int32_t>, item_mix_grade_weight_size> weights;

                for (int32_t i = 0; i < item_mix_grade_weight_size; ++i)
                {
                    const int32_t value = (*weightData)[i];
                    if (value != 0)
                    {
                        const int32_t weight = prev + value;
                        weights.emplace_back(static_cast<ItemMixGradeWeightType>(i), weight);

                        prev = weight;
                    }
                }

                assert(!weights.empty());

                const ItemMixGradeWeightType grade = [&]() -> ItemMixGradeWeightType
                    {
                        std::uniform_int_distribution dist(0, weights.back().second);

                        const int32_t rand = dist(_mt);

                        for (const auto& [type, value] : weights)
                        {
                            if (rand <= value)
                            {
                                return type;
                            }
                        }

                        assert(false);

                        return weights.begin()->first;
                    }();

                const ItemData* itemData = [&]() -> const ItemData*
                    {
                        const std::array<const ItemData*, item_mix_grade_count>& dataList = itemMixData->GetResultItemDataList();

                        switch (grade)
                        {
                        case ItemMixGradeWeightType::Low:
                            return dataList[static_cast<int32_t>(ItemMixGradeType::Low)];
                        case ItemMixGradeWeightType::Middle:
                            return dataList[static_cast<int32_t>(ItemMixGradeType::Middle)];
                        case ItemMixGradeWeightType::High:
                            return dataList[static_cast<int32_t>(ItemMixGradeType::High)];
                        case ItemMixGradeWeightType::Super:
                            return dataList[static_cast<int32_t>(ItemMixGradeType::Super)];
                        case ItemMixGradeWeightType::Fail:
                        case ItemMixGradeWeightType::Count:
                        default:;
                        }

                        return nullptr;
                    }();

                if (itemData)
                {
                    player.Send(ItemMixMessageCreator::CreateItemMixSuccess(group.GetId(), itemData->GetId(), 1, 0));
                }
                else
                {
                    player.Send(ItemMixMessageCreator::CreateItemMixFailure(group.GetId(), 1, 0));
                }

                result = true;


            } while (false);

            if (!result)
            {
                SUNLIGHT_LOG_ERROR(_serviceLocator, log);

                player.Send(ItemMixMessageCreator::CreateItemMixSuccess(group.GetId(), itemId, 1, 0));
            }
        }
        break;
        }

        return false;
    }

    void PlayerGroupSystem::SpawnStoredItem(const GameGroup& group, GamePlayer& player, const GameItem& item, int32_t page, int32_t price, int32_t offset)
    {
        const SceneObjectComponent& playerSceneObjectComponent = player.GetSceneObjectComponent();
        const Eigen::Vector2f& playerPosition = playerSceneObjectComponent.GetPosition();
        const float playerYaw = playerSceneObjectComponent.GetYaw();

        const auto [pos, yaw] = CalculateStoredItemPosition(playerPosition, playerYaw, offset);

        auto storedItem = std::make_shared<GameStoredItem>(_serviceLocator.Get<GameEntityIdPublisher>(),
            player.GetId(), player.GetType(), item.GetData(), item.GetQuantity(), group.GetId(), page, price, offset);

        auto sceneObjectComponent = std::make_unique<SceneObjectComponent>();
        sceneObjectComponent->SetPosition(pos);
        sceneObjectComponent->SetDestPosition(pos);
        sceneObjectComponent->SetYaw(yaw);

        storedItem->AddComponent(std::move(sceneObjectComponent));

        player.GetComponent<StreetVendorHostComponent>().AddStoredItem(page, storedItem->GetId());
        Get<SceneObjectSystem>().SpawnItem(std::move(storedItem));
    }

    auto PlayerGroupSystem::FindGroup(int32_t groupId) -> GameGroup*
    {
        const auto iter = _gameGroups.find(groupId);

        return iter != _gameGroups.end() ? iter->second.get() : nullptr;
    }

    auto PlayerGroupSystem::FindGroup(int32_t groupId) const -> const GameGroup*
    {
        const auto iter = _gameGroups.find(groupId);

        return iter != _gameGroups.end() ? iter->second.get() : nullptr;
    }

    auto PlayerGroupSystem::CalculateStoredItemPosition(Eigen::Vector2f ownerPos, float ownerYaw, int64_t slot) -> std::pair<Eigen::Vector2f, float>
    {
        assert(slot >= 0 && slot < GameConstant::MAX_STREET_VENDOR_STORED_ITEM_SIZE);

        const Eigen::AngleAxisf axis((ownerYaw - 90.f + (static_cast<float>(slot - 1) * 45.f)) * static_cast<float>(std::numbers::pi) / 180.f, Eigen::Vector3f::UnitZ());
        const auto rotation = axis.toRotationMatrix();

        const auto pos = rotation * Eigen::Vector3f(0.f, 30.f, 0.f);

        return std::make_pair(ownerPos + Eigen::Vector2f(pos.x(), pos.y()), ownerYaw);
    }
}
