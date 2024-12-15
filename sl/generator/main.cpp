#include "sl/generator/sunlight_generator.h"

#pragma warning( push )
#pragma warning( disable : 4100 )
#include "sl/generator/api/generated/request.pb.h"
#pragma warning( pop )

int main(int argc, char* argv[])
{
    sunlight::AuthenticationRequest req;

    try
    {
        sunlight::SunlightGenerator generator;

        return generator.Run(std::span(argv, argc));
    }
    catch (const std::exception& e)
    {
        std::cerr << fmt::format("fail to run generator. exception: {}", e.what());
    }

    return EXIT_FAILURE;
}
