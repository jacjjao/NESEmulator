#include <cstdlib>
#include "NES.hpp"


int main()
{   
    auto cartridge = std::make_shared<Cartridge>();
    //if (!cartridge->loadiNESFile("C:/Users/user/Desktop/hi/C++/NESEmulator/rom_singles/04-zero_page.nes"))
    if (!cartridge->loadiNESFile("C:/Users/user/Desktop/hi/C++/NESEmulator/Donkey Kong.nes"))
    {
        return EXIT_FAILURE;
    }

    NES nes;
    nes.insertCartridge(cartridge);
    nes.run();

    return EXIT_SUCCESS;
}