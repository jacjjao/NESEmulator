#include <cstdlib>
#include "NES.hpp"


int main()
{   
    auto cartridge = std::make_shared<Cartridge>();
    // if (!cartridge->loadiNESFile("C:/Users/user/Desktop/hi/C++/NESEmulator/cpu_test_rom/official_only.nes"))
    // if (!cartridge->loadiNESFile("C:/Users/user/Desktop/hi/C++/NESEmulator/ppu_test_rom/sprite_hit_tests/01.basics.nes"))
    if (!cartridge->loadiNESFile("../../../The Legend of Zelda.nes"))
    {
        return EXIT_FAILURE;
    }

    NES nes;
    nes.insertCartridge(cartridge);
    nes.run();

    return EXIT_SUCCESS;
}