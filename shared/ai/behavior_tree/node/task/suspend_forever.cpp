#include "suspend_forever.h"

namespace sunlight::bt::node
{
    auto SuspendForever::Run() -> Result
    {
        struct Foo{};

        [[maybe_unused]]
        const auto va = co_await bt::Event<SuspendForever>();

        co_return false;
    }

    auto SuspendForever::GetName() const -> std::string_view
    {
        return name;
    }

    void from_xml(SuspendForever&, const pugi::xml_node&)
    {
    }
}
