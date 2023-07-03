#include <SDL2/SDL.h>

#include <bit>
#include <cstdlib>
#include <fstream>
#include <algorithm>
#include <iostream>
#include <bitset>

#include "CPU6502.hpp"


int main()
{
    static_assert(std::endian::native == std::endian::little);

    std::ifstream test_file { "C:/Users/user/Desktop/hi/C++/NESEmulator/nestest.nes", std::ios::binary };
    if (!test_file.is_open())
        return EXIT_FAILURE;
    CPU6502 cpu;
    std::vector<u8> data(std::istreambuf_iterator<char>(test_file), {});
    // 16KB PRG ROM
    std::copy(
        data.cbegin() + 16,
        data.cbegin() + 16 + 16384,
        &cpu.mem[0xC000]
    );
    cpu.reg.PC = 0xC000;
    cpu.reg.Status = 0x24;
    cpu.mem[0x02] = 0;
    cpu.mem[0x03] = 0;

    while (true)
    {
        cpu.update();
        if (cpu.mem[0x02] > 0) 
        {
            std::cout << "Cpu test failed 0x02: " << cpu.mem[0x02] << '\n';
            break;
        }
        if (cpu.mem[0x03] > 0)
        {
            std::cout << "Cpu test failed 0x03: " << cpu.mem[0x03] << '\n';
            break;
        }
        if (cpu.reg.PC == 0xC6BD)
            cpu.reg.PC = 0xC6C7;
        else if (cpu.reg.PC == 0xC6C9)
            cpu.reg.PC = 0xC6D0;
        else if (cpu.reg.PC == 0xC6D2)
            cpu.reg.PC = 0xC6E2;
        else if (cpu.reg.PC == 0xC6E4)
            cpu.reg.PC = 0xC6F0;
        else if (cpu.reg.PC == 0xC6F2)
            cpu.reg.PC = 0xC708;
    }
    
    return EXIT_SUCCESS;
}