#include <cstdlib>
#include <fstream>
#include <iostream>

#include "NES.hpp"

void cpu_test()
{
    std::ifstream test_file { "C:/Users/user/Desktop/hi/C++/NESEmulator/nestest.nes", std::ios::binary };
    if (!test_file.is_open())
        return;
    Bus bus;
    std::vector<u8> data(std::istreambuf_iterator<char>(test_file), {});
    // 16KB PRG ROM
    int i = 0;
    for (auto it = data.cbegin() + 16; i < 16384; ++i, ++it)
    {
        bus.cpu.write(0x8000 + i, *it);
        bus.cpu.write(0xC000 + i, *it);
    }

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
    }
}

void cartridge_and_mapper_test()
{
    Bus bus;

    auto cartridge = std::make_shared<Cartridge>();
    cartridge->loadiNESFile("C:/Users/user/Desktop/hi/C++/NESEmulator/nestest.nes");
    
    bus.insertCartridge(cartridge);
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
    }
}

int main()
{  
    auto cartridge = std::make_shared<Cartridge>();
    cartridge->loadiNESFile("C:/Users/user/Desktop/hi/C++/NESEmulator/nestest.nes");

    NES nes;
    nes.insertCartridge(cartridge);
    nes.run();
    
    return EXIT_SUCCESS;
}