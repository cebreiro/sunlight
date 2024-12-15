#pragma once
#include "sl/generator/service/community/notification/community_notification_type.h"

namespace sunlight
{
    class ICommunityNotification
    {
    public:
        virtual ~ICommunityNotification() = default;

        virtual auto GetType() const -> CommunityNotificationType = 0;
    };

    template <typename T>
    concept community_notification_concept = requires (T t)
    {
        requires std::derived_from<T, ICommunityNotification>;
        requires std::same_as<std::remove_cv_t<decltype(T::TYPE)>, CommunityNotificationType>;
        requires std::same_as<typename community_notification_type_mapping<T::TYPE>::type, T>;
    };

    template <community_notification_concept T>
    auto Cast(ICommunityNotification& command) -> T*
    {
        if (command.GetType() == T::TYPE)
        {
            T* result = static_cast<T*>(&command);
            assert(dynamic_cast<T*>(&command) == result);

            return result;
        }

        return nullptr;
    }

    template <community_notification_concept T>
    auto Cast(const ICommunityNotification& command) -> const T*
    {
        if (command.GetType() == T::TYPE)
        {
            const T* result = static_cast<const T*>(&command);
            assert(dynamic_cast<const T*>(&command) == result);

            return result;
        }

        return nullptr;
    }

    template <typename TVisitor>
    void Visit(const TVisitor& visitor, const ICommunityNotification& command)
    {
        const CommunityNotificationType type = command.GetType();

        boost::mp11::mp_for_each<boost::describe::describe_enumerators<CommunityNotificationType>>(
            [&visitor, type, &command](auto describe)
            {
                if (type == static_cast<CommunityNotificationType>(describe.value))
                {
                    using command_type = typename community_notification_type_mapping<static_cast<CommunityNotificationType>(describe.value)>::type;

                    visitor.template operator()<command_type>(*Cast<command_type>(command));
                }
            });
    }
}
