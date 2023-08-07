#include <cstdlib>
#include "NES.hpp"
#include "Mapper/AllMapper.hpp"


std::unique_ptr<Mapper> createCartridge(const std::filesystem::path& path)
{
    Cartridge cart;
    u8 mapper_type = cart.loadiNESFile(path);
    std::unique_ptr<Mapper> mapper;
    switch (mapper_type)
    {
    case 000:
        mapper.reset(new Mapper000{ std::move(cart) });
        break;

    case 001:
        mapper.reset(new Mapper001{ std::move(cart) });
        break;

    case 002:
        mapper.reset(new Mapper002{ std::move(cart) });
        break;

    case 003:
        mapper.reset(new Mapper003{ std::move(cart) });
        break;

    default:
    {
        char err_msg[50] = "";
        std::sprintf(err_msg, "Unsupport mapper %03d", (int)mapper_type);
        throw std::runtime_error{ err_msg };
    }
    }
    std::printf("Mapper %03d\n", (int)mapper_type);
    return mapper;
}

int main()
{   
    try
    {
        // auto cartridge = createCartridge("C:/Users/user/Desktop/hi/C++/NESEmulator/cpu_test_rom/instr_timing.nes");
        // auto cartridge = createCartridge("C:/Users/user/Desktop/hi/C++/NESEmulator/ppu_test_rom/sprite_hit_tests/01.basics.nes");
        // auto cartridge = createCartridge("C:/Users/user/Desktop/hi/C++/NESEmulator/mapper_test_rom/mmc3_test_2/5-MMC3.nes");
        auto cartridge = createCartridge("../../../Mega Man.nes");
        
        NES nes;
        nes.bus.insertCartridge(std::move(cartridge));
        nes.run();
    }
    catch (std::exception& e)
    {
        std::fprintf(stderr, "%s\n", e.what());
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}