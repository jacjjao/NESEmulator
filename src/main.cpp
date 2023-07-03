#include <SDL2/SDL.h>

#include <bit>
#include <cstdlib>
#include <fstream>
#include <algorithm>
#include <iostream>
#include <bitset>

#include "Bus.hpp"


int main()
{
    static_assert(std::endian::native == std::endian::little);

    std::ifstream test_file { "C:/Users/user/Desktop/hi/C++/NESEmulator/nestest.nes", std::ios::binary };
    if (!test_file.is_open())
        return EXIT_FAILURE;
    Bus bus;
    std::vector<u8> data(std::istreambuf_iterator<char>(test_file), {});
    // 16KB PRG ROM
    int i = 0;
    for (auto it = data.cbegin() + 16; i < 16384; ++i, ++it)
        bus.cpu.write(0xC000 + i, *it);
    std::cout << "Hi\n";
    bus.cpu.reg_.PC = 0xC000;
    bus.cpu.reg_.Status = 0x24;

    while (true)
    {
        bus.cpu.update();
        if (bus.cpu.read(0x02) > 0)
        {
            std::cout << "Cpu test failed 0x02: " << bus.cpu.read(0x02) << '\n';
            break;
        }
        if (bus.cpu.read(0x03) > 0)
        {
            std::cout << "Cpu test failed 0x03: " << bus.cpu.read(0x03) << '\n';
            break;
        }
        if (bus.cpu.reg_.PC == 0xC6BD)
            bus.cpu.reg_.PC = 0xC6C7;
        else if (bus.cpu.reg_.PC == 0xC6C9)
            bus.cpu.reg_.PC = 0xC6D0;
        else if (bus.cpu.reg_.PC == 0xC6D2)
            bus.cpu.reg_.PC = 0xC6E2;
        else if (bus.cpu.reg_.PC == 0xC6E4)
            bus.cpu.reg_.PC = 0xC6F0;
        else if (bus.cpu.reg_.PC == 0xC6F2)
            bus.cpu.reg_.PC = 0xC708;
    }
    
    return EXIT_SUCCESS;
}