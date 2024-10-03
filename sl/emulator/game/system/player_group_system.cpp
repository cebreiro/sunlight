#include "player_group_system.h"

#include "sl/emulator/game/component/player_group_component.h"
#include "sl/emulator/game/component/player_item_component.h"
#include "sl/emulator/game/contants/group/game_group.h"
#include "sl/emulator/game/entity/game_player.h"
#include "sl/emulator/game/message/zone_community_message.h"
#include "sl/emulator/game/message/zone_message.h"
#include "sl/emulator/game/message/creator/game_player_message_creator.h"
#include "sl/emulator/game/message/creator/item_archive_message_creator.h"
#include "sl/emulator/game/message/creator/item_trade_message_creator.h"
#include "sl/emulator/game/system/entity_view_range_system.h"
#include "sl/emulator/game/system/item_trade_system.h"
#include "sl/emulator/game/zone/stage.h"
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
        case GameGroupType::Unk04:
        case GameGroupType::Unk05:
        case GameGroupType::ItemMix:
        case GameGroupType::StreetVendor:
        case GameGroupType::Interior:
        default:
            SUNLIGHT_LOG_WARN(_serviceLocator,
                fmt::format("[{}] unhandled player group. player: {}, group: {}",
                    GetName(), player.GetCId(), ToString(groupType)));
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
            }
            break;
            case GameGroupType::Null:
            case GameGroupType::Unk04:
            case GameGroupType::Unk05:
            case GameGroupType::ItemMix:
            case GameGroupType::StreetVendor:
            case GameGroupType::Interior:
            default:
                assert(false);
            }
        }
        break;
        case 900:
        {
            if (group.GetType() != GameGroupType::Trade)
            {
                assert(false);

                return;
            }

            const int32_t tradeMessage = reader.Read<int32_t>();
            switch (tradeMessage)
            {
            case 1601:
            {
                if (group.IsHost(player))
                {
                    OnHostExit(group, player);

                    return;
                }
                else
                {
                    OnGuestExit(group, player);

                    return;
                }
            }
            break;
            case 1001: // lift item
            {
                const auto [targetItemId, targetItemType] = reader.ReadInt64();

                if (!Get<ItemTradeSystem>().LiftItem(group, player, game_entity_id_type(targetItemId)))
                {
                    player.GetClient().Disconnect();

                    return;
                }
            }
            break;
            case 1002: // lower item
            {
                const int32_t x = reader.Read<int32_t>();
                const int32_t y = reader.Read<int32_t>();
                const auto [targetItemId, targetItemType] = reader.ReadInt64();

                if (!Get<ItemTradeSystem>().LowerItem(group, player, x, y, game_entity_id_type(targetItemId)))
                {
                    player.GetClient().Disconnect();

                    return;
                }
            }
            break;
            case 1003:
            {
                const int32_t gold = reader.Read<int32_t>();

                if (!Get<ItemTradeSystem>().ChangeGold(group, player, gold))
                {
                    player.GetClient().Disconnect();

                    return;
                }
            }
            break;
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

                    return;
                }
            }
            break;
            }
        }
        break;
        default:
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
        else
        {
            assert(false);
        }
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
}
