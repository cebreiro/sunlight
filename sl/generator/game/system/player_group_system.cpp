#include "player_group_system.h"

#include "sl/generator/game/component/player_group_component.h"
#include "sl/generator/game/component/scene_object_component.h"
#include "sl/generator/game/component/street_vendor_host_component.h"
#include "sl/generator/game/contents/group/game_group.h"
#include "sl/generator/game/contents/group/group_message_handler.hpp"
#include "sl/generator/game/entity/game_item.h"
#include "sl/generator/game/entity/game_player.h"
#include "sl/generator/game/entity/game_stored_item.h"
#include "sl/generator/game/message/zone_community_message.h"
#include "sl/generator/game/message/zone_message.h"
#include "sl/generator/game/message/creator/game_player_message_creator.h"
#include "sl/generator/game/message/creator/item_mix_message_creator.h"
#include "sl/generator/game/message/creator/item_trade_message_creator.h"
#include "sl/generator/game/message/creator/street_vendor_message_creator.h"
#include "sl/generator/game/system/entity_view_range_system.h"
#include "sl/generator/game/system/item_archive_system.h"
#include "sl/generator/game/system/item_trade_system.h"
#include "sl/generator/game/system/scene_object_system.h"
#include "sl/generator/game/zone/stage.h"
#include "sl/generator/game/zone/service/game_entity_id_publisher.h"
#include "sl/generator/server/client/game_client.h"

namespace sunlight
{
    PlayerGroupSystem::PlayerGroupSystem(const ServiceLocator& serviceLocator, int32_t stageId)
        : _serviceLocator(serviceLocator)
        , _stageId(stageId)
        , _itemMixMessageHandlers([]() -> std::unordered_map<int32_t, UniquePtrNotNull<IGroupMessageHandler>>
            {
                std::unordered_map<int32_t, UniquePtrNotNull<IGroupMessageHandler>> result;

                result.try_emplace(1601, std::make_unique<ItemMixExitMessageHandler>());
                result.try_emplace(1105, std::make_unique<ItemMixTryMessageHandler>());
                result.try_emplace(1106, std::make_unique<ItemMixExitMessageHandler>());
                result.try_emplace(1108, std::make_unique<ItemMixChangeStepMessageHandler>());
                result.try_emplace(1150, std::make_unique<ItemMixLiftItemMessageHandler>());
                result.try_emplace(1151, std::make_unique<ItemMixLowerItemMessageHandler>());

                return result;
            }())
        , _itemTradeMessageHandlers([]() -> std::unordered_map<int32_t, UniquePtrNotNull<IGroupMessageHandler>>
            {
                std::unordered_map<int32_t, UniquePtrNotNull<IGroupMessageHandler>> result;

                result.try_emplace(1601, std::make_unique<ItemTradeExitMessageHandler>());
                result.try_emplace(1001, std::make_unique<ItemTradeLiftItemMessageHandler>());
                result.try_emplace(1002, std::make_unique<ItemTradeLowerItemMessageHandler>());
                result.try_emplace(1003, std::make_unique<ItemTradeChangeGoldMessageHandler>());
                result.try_emplace(1005, std::make_unique<ItemTradeConfirmMessageHandler>());

                return result;
            }())
        , _streetVendorMessageHandlers([]() -> std::unordered_map<int32_t, UniquePtrNotNull<IGroupMessageHandler>>
            {
                std::unordered_map<int32_t, UniquePtrNotNull<IGroupMessageHandler>> result;

                result.try_emplace(1601, std::make_unique<StreetVendorExitMessageHandler>());
                result.try_emplace(1302, std::make_unique<StreetVendorClickItemMessageHandler>());
                result.try_emplace(1304, std::make_unique<StreetVendorChangePageMessageHandler>());
                result.try_emplace(1305, std::make_unique<StreetVendorChangePriceMessageHandler>());
                result.try_emplace(1306, std::make_unique<StreetVendorDisplayMessageHandler>());
                result.try_emplace(1308, std::make_unique<StreetVendorPurchaseMessageHandler>());

                return result;
            }())
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

    auto PlayerGroupSystem::GetServiceLocator() const -> const ServiceLocator&
    {
        return _serviceLocator;
    }

    auto PlayerGroupSystem::GetStageId() const -> int32_t
    {
        return _stageId;
    }

    auto PlayerGroupSystem::GetRandomEngine() -> std::mt19937&
    {
        return _mt;
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
            ProcessHostExit(*iter->second, player);
        }
        else
        {
            ProcessGuestExit(*iter->second, player);
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
        [[maybe_unused]] const int32_t hostType = objectReader.Read<int32_t>();
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
        int32_t subType = 0;

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
            subType = reader.Read<int32_t>();

            switch (group.GetType())
            {
            case GameGroupType::Trade:
            {
                if (const auto iter2 = _itemTradeMessageHandlers.find(subType);
                    iter2 != _itemTradeMessageHandlers.end())
                {
                    handled = true;

                    iter2->second->Handle(*this, group, player, reader);
                }
            }
            break;
            case GameGroupType::StreetVendor:
            {
                if (const auto iter2 = _streetVendorMessageHandlers.find(subType);
                    iter2 != _streetVendorMessageHandlers.end())
                {
                    handled = true;

                    iter2->second->Handle(*this, group, player, reader);
                }
            }
            break;
            case GameGroupType::ItemMix:
            {
                if (const auto iter2 = _itemMixMessageHandlers.find(subType);
                    iter2 != _itemMixMessageHandlers.end())
                {
                    handled = true;

                    iter2->second->Handle(*this, group, player, reader);
                }
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
                fmt::format("[{}] unhandled group message. player: {}, group: {}, message_type: {}, subType: {}, buffer: {}",
                    GetName(), player.GetCId(), groupId, messageType, subType, reader.GetBuffer().ToString()));
        }
    }

    void PlayerGroupSystem::ProcessHostExit(GameGroup& group, GamePlayer& host)
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

    void PlayerGroupSystem::ProcessGuestExit(GameGroup& group, GamePlayer& guest)
    {
        assert(group.IsGuest(guest));

        if (group.GetType() == GameGroupType::Trade)
        {
            ProcessHostExit(group, group.GetHost());

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

    void PlayerGroupSystem::ProcessTradeComplete(GameGroup& group, GamePlayer& host, GamePlayer& guest)
    {
        host.GetGroupComponent().Clear();
        guest.GetGroupComponent().Clear();

        _gameGroups.erase(group.GetId());
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
