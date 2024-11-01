#include "sl/emulator/emulator.h"

int main(int argc, char* argv[])
{
    try
    {
        sunlight::SlEmulator emulator;

        return emulator.Run(std::span(argv, argc));
    }
    catch (const std::exception& e)
    {
        std::cerr << fmt::format("fail to run emulator. exception: {}", e.what());
    }

    return EXIT_FAILURE;
}
