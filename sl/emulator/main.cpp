#include "sl/emulator/emulator.h"

int main(int argc, char* argv[])
{
    sunlight::SlEmulator emulator;

    return emulator.Run(std::span(argv, argc));
}
