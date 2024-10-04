#include "player_group_system.h"

#include "sl/emulator/game/component/player_group_component.h"
#include "sl/emulator/game/component/player_item_component.h"
#include "sl/emulator/game/component/scene_object_component.h"
#include "sl/emulator/game/component/street_vendor_host_component.h"
#include "sl/emulator/game/contants/group/game_group.h"
#include "sl/emulator/game/entity/game_item.h"
#include "sl/emulator/game/entity/game_player.h"
#include "sl/emulator/game/entity/game_stored_item.h"
#include "sl/emulator/game/message/zone_community_message.h"
#include "sl/emulator/game/message/zone_message.h"
#include "sl/emulator/game/message/creator/game_player_message_creator.h"
#include "sl/emulator/game/message/creator/item_trade_message_creator.h"
#include "sl/emulator/game/message/creator/street_vendor_message_creator.h"
#include "sl/emulator/game/system/entity_view_range_system.h"
#include "sl/emulator/game/system/item_archive_system.h"
#include "sl/emulator/game/system/item_trade_system.h"
#include "sl/emulator/game/system/scene_object_system.h"
#include "sl/emulator/game/zone/stage.h"
#include "sl/emulator/game/zone/service/game_entity_id_publisher.h"
#include "sl/emulator/server/client/game_client.h"

namespace sunlight
{
    PlayerGroupSystem::PlayerGroupSystem(const ServiceLocator& serviceLocator)
        : _serviceLocator(serviceLocator)
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

    void PlayerGroupSystem::HandleStateProposition(const ZoneMessage& message)
    {
        // client's group state broadcast request

        GamePlayer& player = message.player;
        SlPacketReader& reader = message.reader;

        const std::string& title = reader.ReadString();
        const int32_t groupId = reader.Read<int32_t>();
        const GameGroupType groupType = static_cast<GameGroupType>(reader.Read<int32_t>());
        const int32_t unk2 = reader.Read<int32_t>(); // maybe on/off

        constexpr bool includeSelf = true;

        Get<EntityViewRangeSystem>().Broadcast(player,
            GamePlayerMessageCreator::CreatePlayerStateProposition(player, title, groupId, groupType, unk2),
            includeSelf);
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
        case GameGroupType::Unk04:
        case GameGroupType::Unk05:
        case GameGroupType::ItemMix:
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

                    player.Send(ItemTradeMessageCreator::CreateGroupHostData(group.GetId(), host));

                    Get<ItemTradeSystem>().Start(player);
                }

                handled = true;
            }
            break;
            case GameGroupType::Null:
            case GameGroupType::Unk04:
            case GameGroupType::Unk05:
            case GameGroupType::ItemMix:
            case GameGroupType::StreetVendor:
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
            case GameGroupType::Null:
            case GameGroupType::Unk04:
            case GameGroupType::Unk05:
            case GameGroupType::ItemMix:
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

            for (int64_t i = 0; i < GameConstant::MAX_STREET_VENDOR_STORED_ITEM_SIZE; ++i)
            {
                if (const std::optional<game_entity_id_type> id = streetVendorHostComponent.GetStoredItem(i); id.has_value())
                {
                    [[maybe_unused]]
                    const bool removed = Get<SceneObjectSystem>().RemoveStoredItem(*id);
                    assert(removed);
                }
            }

            host.RemoveComponent<StreetVendorHostComponent>();
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
            ProcessTradeFail(group, guest);

            group.RemoveGuest(guest);

            GamePlayer& host = group.GetHost();

            host.Send(ItemTradeMessageCreator::CreateGroupGuestExit(group.GetId(), host));
        }
        else if (group.GetType() == GameGroupType::StreetVendor)
        {
            assert(false);
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

                ItemTradeSystem& itemTradeSystem = Get<ItemTradeSystem>();

                if (itemTradeSystem.Commit(host, guest))
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

            player.Send(StreetVendorMessageCreator::CreatePageItemDisplay(group.GetId(), (page / 2) * 2, player));

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

            std::array<std::pair<const GameItem*, int32_t>, GameConstant::MAX_STREET_VENDOR_STORED_ITEM_SIZE> storedItemData = {};
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
                        storedItemData[k++] = std::pair{ saleItem, price };
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

            const SceneObjectComponent& playerSceneObjectComponent = player.GetSceneObjectComponent();
            const Eigen::Vector2f& playerPosition = playerSceneObjectComponent.GetPosition();
            const float playerYaw = playerSceneObjectComponent.GetYaw();

            for (int64_t i = 0; i < std::ssize(storedItemData); ++i)
            {
                const auto [saleItem, price] = storedItemData[i];
                if (saleItem)
                {
                    auto storedItem = std::make_shared<GameStoredItem>(_serviceLocator.Get<GameEntityIdPublisher>(),
                        saleItem->GetData(), saleItem->GetQuantity(), group.GetId(), price);

                    const auto [pos, yaw] = CalculateStoredItemPosition(playerPosition, playerYaw, i);

                    auto sceneObjectComponent = std::make_unique<SceneObjectComponent>();
                    sceneObjectComponent->SetPosition(pos);
                    sceneObjectComponent->SetDestPosition(pos);
                    sceneObjectComponent->SetYaw(yaw);

                    storedItem->AddComponent(std::move(sceneObjectComponent));

                    hostComponent.AddStoredItem(i, storedItem->GetId());
                    Get<SceneObjectSystem>().SpawnItem(std::move(storedItem));
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
        }

        return false;
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
