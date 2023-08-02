#include <cstdlib>
#include "NES.hpp"


int main()
{   
    auto cartridge = std::make_shared<Cartridge>();
    // if (!cartridge->loadiNESFile("C:/Users/user/Desktop/hi/C++/NESEmulator/cpu_test_rom/cpu_dummy_reads.nes"))
    // if (!cartridge->loadiNESFile("C:/Users/user/Desktop/hi/C++/NESEmulator/ppu_test_rom/vbl_clear_time.nes"))
    if (!cartridge->loadiNESFile("../../../Super Mario Bros.nes"))
    {
        return EXIT_FAILURE;
    }

    NES nes;
    nes.insertCartridge(cartridge);
    nes.run();

    return EXIT_SUCCESS;
}