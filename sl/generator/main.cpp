#include "sl/generator/sunlight_generator.h"

int main(int argc, char* argv[])
{
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
