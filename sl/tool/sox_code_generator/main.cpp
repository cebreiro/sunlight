#include "sl/tool/sox_code_generator/sox_code_generator.h"

int main(int argc, char* argv[])
{
    sunlight::SoxCodeGenerator soxCodeGenerator;

    return soxCodeGenerator.Run(std::span(argv, argc));
}
