#include <SDL2/SDL.h>

#include <bit>
#include <cstdlib>

#include "CPU6502.hpp"

int main()
{
    static_assert(std::endian::native == std::endian::little);

    CPU6502 cpu;

    return EXIT_SUCCESS;
}