#include "pch.hpp"
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

    case 004:
        mapper.reset(new Mapper004{ std::move(cart) });
        break;

    case 066:
        mapper.reset(new Mapper066{ std::move(cart) });
        break;

    default:
    {
        char err_msg[50] = "";
        std::snprintf(err_msg, 50, "Unsupport mapper %03d", (int)mapper_type);
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
        NES nes;
        // nes.bus.insertCartridge(createCartridge("C:/Users/user/Desktop/hi/C++/NESEmulator/apu_test_rom/apu_test/apu_test.nes"));
        // nes.bus.insertCartridge(createCartridge("C:/Users/user/Desktop/hi/C++/NESEmulator/cpu_test_rom/branch_timing_tests/3.Forward_Branch.nes"));
        // nes.bus.insertCartridge(createCartridge("C:/Users/user/Desktop/hi/C++/NESEmulator/ppu_test_rom/oam_stress.nes"));
        // nes.bus.insertCartridge(createCartridge("C:/Users/user/Desktop/hi/C++/NESEmulator/mapper_test_rom/mmc3_test_2/5-MMC3.nes"));
        nes.bus.insertCartridge(createCartridge("../../../Super Mario Bros.nes"));
        nes.run();
    }
    catch (std::exception& e)
    {
        std::fprintf(stderr, "%s\n", e.what());
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}