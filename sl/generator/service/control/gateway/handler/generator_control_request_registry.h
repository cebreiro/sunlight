#pragma once

namespace sunlight
{
    class IGeneratorControlRequestHandler;
    class GeneratorControlRequestRouter;
}

namespace sunlight
{
    class GeneratorControlRequestRegistry
    {
    public:
        bool Register(GeneratorControlRequestRouter& router, const ServiceLocator& serviceLocator);

    private:
        template <typename T>
        bool Add(GeneratorControlRequestRouter& router, SharedPtrNotNull<T> handler);

    private:
        std::vector<SharedPtrNotNull<IGeneratorControlRequestHandler>> _handlers;
    };
}
