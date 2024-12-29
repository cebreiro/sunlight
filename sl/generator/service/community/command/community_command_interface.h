#pragma once
#include "sl/generator/service/community/command/community_command_type.h"

namespace sunlight
{
    class ICommunityCommand
    {
    public:
        virtual ~ICommunityCommand() = default;

        virtual auto GetType() const -> CommunityCommandType = 0;
    };

    template <typename T>
    concept community_command_concept = requires (T t)
    {
        requires std::derived_from<T, ICommunityCommand>;
        requires std::same_as<std::remove_cv_t<decltype(T::TYPE)>, CommunityCommandType>;
        requires std::same_as<typename community_command_type_mapping<T::TYPE>::type, T>;
    };

    template <community_command_concept T>
    auto Cast(ICommunityCommand& command) -> T*
    {
        if (command.GetType() == T::TYPE)
        {
            T* result = static_cast<T*>(&command);
            assert(dynamic_cast<T*>(&command) == result);

            return result;
        }

        return nullptr;
    }

    template <community_command_concept T>
    auto Cast(const ICommunityCommand& command) -> const T*
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
    void Visit(const TVisitor& visitor, const ICommunityCommand& command)
    {
        const CommunityCommandType type = command.GetType();

        boost::mp11::mp_for_each<boost::describe::describe_enumerators<CommunityCommandType>>(
            [&visitor, type, &command](auto describe)
            {
                if (type == static_cast<CommunityCommandType>(describe.value))
                {
                    using command_type = typename community_command_type_mapping<static_cast<CommunityCommandType>(describe.value)>::type;

                    visitor.template operator()<command_type>(*Cast<command_type>(command));
                }
            });
    }
}
